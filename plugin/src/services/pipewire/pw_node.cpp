#include "pw_node.h"

#include <cstdint>
#include <cstring>
#include <utility>

#include <pipewire/keys.h>
#include <pipewire/node.h>
#include <qhash.h>
#include <qhashfunctions.h>
#include <qloggingcategory.h>
#include <qmap.h>
#include <qtypes.h>
#include <spa/node/keys.h>
#include <spa/pod/pod.h>
#include <spa/utils/dict.h>
#include <spa/utils/keys.h>
#include <spa/utils/string.h>

#include "ns_services_pipewire_debug.h"
#include "pw_bindable.h"
#include "pw_common.h"
#include "pw_device.h"
#include "pw_node_bound_audio.h"
#include "pwregistry.h"

namespace ns::services::pipewire {
namespace {
const QMap<const char *, NodeType::Flags> MEDIA_CLASS_FLAG_MAP{
    {"Audio/Sink",          NodeType::AudioSink     },
    {"Audio/Source",        NodeType::AudioSource   },
    {"Audio/Duplex",        NodeType::AudioDuplex   },
    {"Stream/Output/Audio", NodeType::AudioOutStream},
    {"Stream/Input/Audio",  NodeType::AudioInStream },
    {"Video/Sink",          NodeType::VideoSink     },
    {"Video/Source",        NodeType::VideoSource   },
};
}

void Node::pw_node_on_info(void *data, const struct pw_node_info *info) {
  auto *self = static_cast<Node *>(data);

  if ((info->change_mask & PW_NODE_CHANGE_MASK_PROPS)) {
    StringMap properties;
    bool      proAudio = false;
    if (const auto *proAudioStr =
            spa_dict_lookup(info->props, "device.profile.pro"))
    {
      proAudio = spa_atob(proAudioStr);
    }

    if (proAudio != self->m_proAudio) {
      self->m_proAudio = proAudio;
    }

    if (self->m_device) {
      if (const auto *routeDevice =
              spa_dict_lookup(info->props, "card.profile.device"))
      {
        auto ok = false;
        auto id = QString::fromUtf8(routeDevice).toInt(&ok);

        if (!ok) {
          qCCritical(NS_LOG_PIPEWIRE)
              << self << "has an invalid card.profile.device:" << routeDevice;
        }

        self->m_routeDevice = id;
        if (self->m_boundData) self->m_boundData->onDeviceChanged();
      } else {
        qCDebug(NS_LOG_PIPEWIRE)
            << self << "has attached device" << self->m_device
            << "but no card.profile.device property. Using node volume "
               "control.";
      }
    }

    const spa_dict_item *item = nullptr;
    spa_dict_for_each(item, info->props) {
      properties.insert(item->key, item->value);
    }

    self->m_properties = std::move(properties);
    emit self->propertiesChanged();
  }

  if (self->m_boundData) self->m_boundData->onInfo(info);

  if (!self->m_ready && !self->m_syncSeq) {
    // TODO
  }
}

void Node::pw_node_on_param(void *data, int /*seq*/, uint32_t id,
    uint32_t index, uint32_t /*next*/, const struct spa_pod *param) {
  auto *self = static_cast<Node *>(data);
  if (self->m_boundData) {
    self->m_boundData->onSpaParam(id, index, param);
  }
}

const struct pw_node_events Node::NODE_EVENTS = {
    .version = PW_VERSION_NODE_EVENTS,
    .info    = pw_node_on_info,
    .param   = pw_node_on_param};

Node::Node(PwBindableProps props) : PwBindableBase(std::move(props)) {}

bool Node::shouldUseDevice() const {
  if (!m_device || m_proAudio || m_routeDevice == -1) return false;
  return m_device->hasRouteDevice(m_routeDevice);
}

QString Node::name() const { return m_name; }

void Node::bindHooks() {
  pw_node_add_listener(
      m_interface.proxy(), &m_listener.hook, &Node::NODE_EVENTS, this);
}
void Node::unbindHooks() {
  if (m_ready) {
    m_ready = false;
    emit readyChanged();
  }

  m_syncSeq = 0;
  m_listener.remove();
  m_routeDevice = -1;
  m_properties.clear();
  emit propertiesChanged();

  if (m_boundData) {
    m_boundData->onUnbind();
  }
}

void Node::onCoreSync(quint32 id, qint32 seq) {
  if (id != m_id || seq != m_syncSeq) return;
  m_ready   = true;
  m_syncSeq = 0;
  emit readyChanged();
}

void Node::initProps(const spa_dict *props) {
  if (const auto *mediaClass = spa_dict_lookup(props, SPA_KEY_MEDIA_CLASS)) {
    if (strcmp(mediaClass, "Audio/Sink") == 0) {
      for (auto [key, value] : MEDIA_CLASS_FLAG_MAP.asKeyValueRange()) {
        if (strcmp(mediaClass, key) == 0) {
          m_type = value;
        }
      }
    }
  }

  if (const auto *nodeName = spa_dict_lookup(props, SPA_KEY_NODE_NAME)) {
    m_name = nodeName;
  }

  if (const auto *nodeDesc = spa_dict_lookup(props, SPA_KEY_NODE_DESCRIPTION)) {
    m_description = nodeDesc;
  }

  if (const auto *nodeNick = spa_dict_lookup(props, PW_KEY_NODE_NICK)) {
    m_nick = nodeNick;
  }

  if (const auto *nodeCategory = spa_dict_lookup(props, PW_KEY_MEDIA_CATEGORY))
  {
    if (strcmp(nodeCategory, "Monitor") == 0 ||
        strcmp(nodeCategory, "Manager") == 0)
    {
      m_isMonitor = true;
    }
  }

  if (const auto *serial = spa_dict_lookup(props, PW_KEY_OBJECT_SERIAL)) {
    auto ok    = false;
    auto value = QString::fromUtf8(serial).toULongLong(&ok);
    if (!ok) {
      qCWarning(NS_LOG_PIPEWIRE)
          << this << "has invalid object.serial property:" << serial;
    } else {
      m_objectSerial = value;
    }
  }

  if (const auto *deviceId = spa_dict_lookup(props, PW_KEY_DEVICE_ID)) {
    auto ok = false;
    auto id = QString::fromUtf8(deviceId).toInt(&ok);
    if (!ok) {
      qCCritical(NS_LOG_PIPEWIRE)
          << this << "has invalid device.id property:" << deviceId;
    } else {
      m_device = m_registry->m_devices.value(id);
      if (!m_device) {
        qCCritical(NS_LOG_PIPEWIRE) << this
                                    << "has a device.id property that does not "
                                       "match any known device IDs. Id:"
                                    << id;
      }
    }
  }

  if (m_type.testFlags(NodeType::Audio)) {
    m_boundData = new NodeBoundAudio(this);
  }
}
} // namespace ns::services::pipewire
