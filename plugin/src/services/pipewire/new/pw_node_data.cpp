#include "pw_node_data.h"

#include <numeric>

#include <qobject.h>
#include <spa/param/props.h>

#include "ns_pipewire_debug.h"
#include "pw_device.h"
#include "pw_node.h"

namespace ns::pipewire {
NodeAudioData::NodeAudioData(Node *node) : QObject(node), m_parent(node) {
  if (auto dev = node->getDevice()) {
    QObject::connect(
        dev, &Device::deviceReady, this, &NodeAudioData::onDeviceReady);

    QObject::connect(dev, &Device::routeVolumesChanged, this,
        &NodeAudioData::onDeviceVolumesChanged);
  }
}

NodeAudioData::~NodeAudioData() {}

void NodeAudioData::onDeviceChanged() {
  auto o = m_parent->getDevice()->getVolumeProps(m_parent->routeDevice());
  if (o.has_value()) {
    updateVolumeProps(o.value());
  }
}

void NodeAudioData::onInfo(const pw_node_info *info) {
  if (info->change_mask & PW_NODE_CHANGE_MASK_PARAMS) {
    for (quint32 i = 0; i < info->n_params; i++) {
      auto &param = info->params[i];

      if (param.id == SPA_PARAM_Props) {
        if ((param.flags & SPA_PARAM_INFO_READWRITE) ==
            SPA_PARAM_INFO_READWRITE)
        {
          pw_node_enum_params(
              m_parent->proxy(), 0, param.id, 0, UINT32_MAX, nullptr);
        } else {
          qCWarning(NS_LOG_PIPEWIRE)
              << "Unable to enumerate props for" << m_parent
              << ": param does not have read/write permissions.";
        }
      }
    }
  }
}

void NodeAudioData::onSpaParam(
    quint32 id, quint32 index, const spa_pod *param) {
  if (id == SPA_PARAM_Props && index == 0) {
    if (m_parent->shouldUseDevice()) {
      return;
    }

    auto props = volume_props_from_spa_pod(param);
    updateVolumeProps(&props);
  }
}

QList<AudioChannel::Enum> NodeAudioData::channels() const { return m_channels; }

float NodeAudioData::averageVolume() const {
  if (m_volumes.isEmpty()) return 0.0f;

  float total = std::accumulate(m_volumes.begin(), m_volumes.end(), 0);
  return total / static_cast<float>(m_volumes.size());
}

void NodeAudioData::setAverageVolume(float value) {
  auto oldAverage = averageVolume();
  auto mul        = oldAverage == 0 ? 0 : value / oldAverage;
  auto volumes    = m_volumes;
  for (auto &volume : volumes) {
    volume = mul == 0 ? value : volume * mul;
  }
  setVolumes(volumes);
}

QList<float> NodeAudioData::volumes() const { return m_volumes; }
void         NodeAudioData::setVolumes(const QList<float> &values) {
  if (m_parent->proxy()) {
    qCCritical(NS_LOG_PIPEWIRE)
        << "Tried to change node volumes for unbound node:" << m_parent;
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
        << "Tried setting volumes for" << m_parent << ", but list size ("
        << realVolumes.size() << ") does not match the size of channels ("
        << m_channels.size() << ")";
    return;
  }

  if (m_parent->shouldUseDevice()) {
    if (m_parent->getDevice()->waitingForDevice()) {
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
        if (!m_parent->getDevice()->setVolumes(
                m_parent->routeDevice(), realVolumes))
        {
          return;
        }

        m_deviceVolumes = realVolumes;
        m_parent->getDevice()->waitForDevice();
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

bool NodeAudioData::isMuted() const { return m_muted; }
void NodeAudioData::setMuted(bool muted) {
  if (!m_parent->proxy()) {
    qCCritical(NS_LOG_PIPEWIRE)
        << "Tried changing mute state for unbound node:" << m_parent;
    return;
  }

  if (muted == m_muted) return;

  if (m_parent->shouldUseDevice()) {
    if (!m_parent->getDevice()->setMuted(m_parent->routeDevice(), muted)) {
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
        << this << "changed mute state of" << m_parent << "to" << muted;
    pw_node_set_param(
        m_parent->proxy(), SPA_PARAM_Props, 0, static_cast<spa_pod *>(pod));
  }

  m_muted = muted;
  emit mutedChanged();
}

void NodeAudioData::onDeviceReady() {
  if (!m_queuedVolumes.isEmpty()) {
    if (m_queuedVolumes != m_deviceVolumes) {
      m_parent->getDevice()->setVolumes(
          m_parent->routeDevice(), m_queuedVolumes);
      m_deviceVolumes = m_queuedVolumes;
      m_volumes       = m_queuedVolumes;
    }

    m_queuedVolumes.clear();
  }
}

void NodeAudioData::onDeviceVolumesChanged(
    qint32 routeDevice, const VolumeProps *volumeProps) {
  if (m_parent->shouldUseDevice() && m_parent->routeDevice() == routeDevice) {
    updateVolumeProps(volumeProps);
  }
}

void NodeAudioData::updateVolumeProps(const VolumeProps *volumeProps) {
  if (volumeProps->volumes.size() != volumeProps->channels.size()) {
    qCWarning(NS_LOG_PIPEWIRE)
        << "Mismatched volume for channels when trying to update volumes for"
        << m_parent;
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
} // namespace ns::pipewire
