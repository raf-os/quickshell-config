#include "pw_node_bound_audio.h"

#include <cstdint>
#include <cstdlib>
#include <numeric>

#include <pipewire/node.h>
#include <pipewire/proxy.h>
#include <qlist.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qtypes.h>
#include <spa/param/param.h>
#include <spa/param/props.h>
#include <spa/pod/builder.h>
#include <spa/pod/vararg.h>
#include <spa/utils/type.h>

#include "ns_services_pipewire_debug.h"
#include "pw_common.h"
#include "pw_device.h"
#include "pw_node.h"

namespace ns::services::pipewire {
NodeBoundAudio::NodeBoundAudio(Node *node) : QObject(node), m_node(node) {
  if (m_node->m_device) {
    QObject::connect(m_node->m_device, &Device::deviceReady, this,
        &NodeBoundAudio::onDeviceReady);

    QObject::connect(m_node->m_device, &Device::routeVolumesChanged, this,
        &NodeBoundAudio::onDeviceVolumesChanged);
  }
}

NodeBoundAudio::~NodeBoundAudio() = default;

void NodeBoundAudio::onDeviceChanged() {
  auto o = m_node->m_device->getVolumeProps(m_node->m_routeDevice);
  if (o.has_value()) {
    updateVolumeProps(o.value());
  }
}

void NodeBoundAudio::onInfo(const pw_node_info *info) {
  if (info->change_mask & PW_NODE_CHANGE_MASK_PARAMS) {
    for (quint32 i = 0; i < info->n_params; i++) {
      auto &param = info->params[i];

      if (param.id == SPA_PARAM_Props) {
        if ((param.flags & SPA_PARAM_INFO_READWRITE) ==
            SPA_PARAM_INFO_READWRITE)
        {
          pw_node_enum_params(
              m_node->m_interface.proxy(), 0, param.id, 0, UINT32_MAX, nullptr);
        } else {
          qCWarning(NS_LOG_PIPEWIRE)
              << "Unable to enumerate props for" << m_node
              << ": param does not have read/write permissions.";
        }
      }
    }
  }
}

void NodeBoundAudio::onSpaParam(
    quint32 id, quint32 index, const spa_pod *param) {
  if (id == SPA_PARAM_Props && index == 0) {
    if (m_node->shouldUseDevice()) {
      return;
    }

    auto props = volume_props_from_spa_pod(param);
    updateVolumeProps(&props);
  }
}

void NodeBoundAudio::onUnbind() {
  m_channels.clear();
  m_volumes.clear();
  m_serverVolumes.clear();
  m_deviceVolumes.clear();
  m_queuedVolumes.clear();
  emit channelsChanged();
  emit volumesChanged();
}

QList<AudioChannel::Enum> NodeBoundAudio::channels() const {
  return m_channels;
}

float NodeBoundAudio::averageVolume() const {
  if (m_volumes.isEmpty()) return 0.0f;

  float total = std::accumulate(m_volumes.begin(), m_volumes.end(), 0);
  return total / static_cast<float>(m_volumes.size());
}
void NodeBoundAudio::setAverageVolume(float value) {
  auto oldAverage = averageVolume();
  auto mul        = oldAverage == 0 ? 0 : value / oldAverage;
  auto volumes    = m_volumes;
  for (auto &volume : volumes) {
    volume = mul == 0 ? value : volume * mul;
  }
  setVolumes(volumes);
}

QList<float> NodeBoundAudio::volumes() const { return m_volumes; }
void         NodeBoundAudio::setVolumes(const QList<float> &values) {
  if (m_node->m_interface.proxy()) {
    qCCritical(NS_LOG_PIPEWIRE)
        << "Tried to change node volumes for unbound node:" << m_node;
    return;
  }

  QList<float> realVolumes;
  realVolumes.reserve(values.size());
  for (const auto &volume : values) {
    realVolumes.append(volume < 0 ? 0 : volume);
  }
  if (realVolumes == m_volumes) return;

  if (realVolumes.size() != m_volumes.size()) {
    qCCritical(NS_LOG_PIPEWIRE)
        << "Tried setting volumes for" << m_node << ", but list size ("
        << realVolumes.size() << ") does not match the size of channels ("
        << m_channels.size() << ")";
    return;
  }

  if (m_node->shouldUseDevice()) {
    if (m_node->m_device->waitingForDevice()) {
      m_queuedVolumes = realVolumes;
    } else {
      bool isChangeSignificant =
          m_serverVolumes.isEmpty() || m_volumeStep == -1;
      if (m_volumeStep != -1) {
        for (auto i = 0; i < m_serverVolumes.size(); i++) {
          auto serverVolume = m_serverVolumes.at(i);
          auto targetVolume = realVolumes.at(i);
          if (targetVolume == 0 ||
              std::abs(targetVolume - serverVolume) >= m_volumeStep)
          {
            isChangeSignificant = true;
            break;
          }
        }
      }

      if (isChangeSignificant) {
        if (!m_node->m_device->setVolumes(m_node->m_routeDevice, realVolumes)) {
          return;
        }

        m_deviceVolumes = realVolumes;
        m_node->m_device->waitForDevice();
      } else {
      }
    }
  } else {
    quint8 buffer[1024];
    auto   builder = SPA_POD_BUILDER_INIT(&buffer, sizeof(buffer));

    auto cubedVolumes = realVolumes;
    for (auto &volume : cubedVolumes) {
      volume *= volume * volume;
    }

    auto pod = spa_pod_builder_add_object(&builder, SPA_TYPE_OBJECT_Props,
        SPA_PARAM_Props, SPA_PROP_channelVolumes,
        SPA_POD_Array(sizeof(float), SPA_TYPE_Float, cubedVolumes.size(),
            cubedVolumes.data()));
  }

  m_volumes = realVolumes;
  emit volumesChanged();
}

bool NodeBoundAudio::isMuted() const { return m_muted; }
void NodeBoundAudio::setMuted(bool muted) {
  if (!m_node->m_interface.proxy()) {
    qCCritical(NS_LOG_PIPEWIRE)
        << "Tried changing mute state for unbound node:" << m_node;
    return;
  }

  if (muted == m_muted) return;

  if (m_node->shouldUseDevice()) {
    if (!m_node->m_device->setMuted(m_node->m_routeDevice, muted)) {
      return;
    }
  } else {
    quint8 buffer[1024];
    auto   builder = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));

    // NOTE: quickshell source this was based off of has a comment here on how
    // there's possibly a memory leak, but to me all seems ok. Macro below does
    // return a void pointer, and pipewire docs are really, REALLY lacking. My
    // best guess is that this is a type-erased pointer to the stack allocated
    // buffer above. Keep an eye on it.
    auto pod = spa_pod_builder_add_object(&builder, SPA_TYPE_OBJECT_Props,
        SPA_PARAM_Props, SPA_PROP_mute, SPA_POD_Bool(muted));

    qCInfo(NS_LOG_PIPEWIRE)
        << this << "changed mute state of" << m_node << "to" << muted;
    pw_node_set_param(m_node->m_interface.proxy(), SPA_PARAM_Props, 0,
        static_cast<spa_pod *>(pod));
  }

  m_muted = muted;
  emit mutedChanged();
}

void NodeBoundAudio::onDeviceReady() {
  if (!m_queuedVolumes.isEmpty()) {
    if (m_queuedVolumes != m_deviceVolumes) {
      m_node->m_device->setVolumes(m_node->m_routeDevice, m_queuedVolumes);
      m_deviceVolumes = m_queuedVolumes;
      m_volumes       = m_queuedVolumes;
    }

    m_queuedVolumes.clear();
  }
}

void NodeBoundAudio::onDeviceVolumesChanged(
    qint32 routeDevice, const VolumeProps *volumeProps) {
  if (m_node->shouldUseDevice() && m_node->m_routeDevice == routeDevice) {
    updateVolumeProps(volumeProps);
  }
}

void NodeBoundAudio::updateVolumeProps(const VolumeProps *volumeProps) {
  if (volumeProps->volumes.size() != volumeProps->channels.size()) {
    qCWarning(NS_LOG_PIPEWIRE)
        << "Mismatched volume for channels when trying to update volumes for"
        << m_node;
    return;
  }

  m_volumeStep = volumeProps->volumeStep;

  bool channelsChanged = false;
  bool volumesChanged  = false;
  bool mutedChanged    = false;

  if (m_channels != volumeProps->channels) {
    m_channels      = volumeProps->channels;
    channelsChanged = true;
  }

  if (m_serverVolumes != volumeProps->volumes) {
    m_serverVolumes = volumeProps->volumes;
  }

  if (m_volumes != volumeProps->volumes) {
    m_volumes      = volumeProps->volumes;
    volumesChanged = true;
  }

  if (m_muted != volumeProps->muted) {
    m_muted      = volumeProps->muted;
    mutedChanged = true;
  }

  if (channelsChanged) emit this->channelsChanged();
  if (volumesChanged) emit this->volumesChanged();
  if (mutedChanged) emit this->mutedChanged();
}
} // namespace ns::services::pipewire
