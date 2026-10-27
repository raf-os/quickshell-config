#include "pw_node.h"

#include <pipewire/keys.h>
#include <pipewire/node.h>
#include <qhashfunctions.h>
#include <qobject.h>
#include <qtypes.h>
#include <spa/node/keys.h>
#include <spa/utils/dict.h>
#include <spa/utils/keys.h>

#include "ns_pipewire_debug.h"
#include "pw_common.h"
#include "pw_device.h"
#include "pw_globals.h"
#include "pw_node_data.h"
#include "pw_registry.h"

namespace ns::pipewire {
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

void Node::bindHooks() {
  pw_node_add_listener(proxy(), &m_hook.hook, &Node::NODE_EVENTS, this);
}

void Node::unbindHooks() {
  if (m_ready) {
    m_ready = false;
    emit readyChanged();
  }

  m_syncSeq = 0;
  m_hook.remove();
  m_routeDevice = -1;
  m_properties.clear();

  if (m_data) {
    delete m_data;
    m_data = nullptr;
  }
}

Device *Node::getDevice() const { return m_device.get(); }
qint32  Node::routeDevice() const { return m_routeDevice; }

void Node::initProps(const string_map_t &props) {
  auto registry = Globals::instance()->registry();
  if (props.contains(SPA_KEY_MEDIA_CLASS)) {
    auto pkey = props.value(SPA_KEY_MEDIA_CLASS);
    for (auto [key, value] : MEDIA_CLASS_FLAG_MAP.asKeyValueRange()) {
      if (pkey == key) {
        m_type = value;
        break;
      }
    }
  }

  if (props.contains(SPA_KEY_NODE_NAME)) {
    m_name = props.value(SPA_KEY_NODE_NAME);
  }

  if (props.contains(SPA_KEY_NODE_DESCRIPTION)) {
    m_description = props.value(SPA_KEY_NODE_DESCRIPTION);
  }

  if (props.contains(PW_KEY_NODE_NICK)) {
    m_nick = props.value(PW_KEY_NODE_NICK);
  }

  if (props.contains(PW_KEY_MEDIA_CATEGORY)) {
    auto nodeCategory = props.value(PW_KEY_MEDIA_CATEGORY);
    if (nodeCategory == "Monitor" || nodeCategory == "Manager") {
      m_isMonitor = true;
    }
  }

  if (props.contains(PW_KEY_OBJECT_SERIAL)) {
    auto ok     = false;
    auto serial = props.value(PW_KEY_OBJECT_SERIAL);
    auto value  = serial.toULongLong(&ok);
    if (!ok) {
      qCWarning(NS_LOG_PIPEWIRE)
          << this << "has invalid object.serial property:" << serial;
    } else {
      m_objectSerial = value;
    }
  }

  if (props.contains(PW_KEY_DEVICE_ID)) {
    auto ok    = false;
    auto devId = props.value(PW_KEY_DEVICE_ID);
    auto id    = devId.toInt(&ok);
    if (!ok) {
      qCCritical(NS_LOG_PIPEWIRE)
          << this << "has invalid device.id property:" << devId;
    } else {
      m_device = registry->getDeviceById(id);
      if (!m_device) {
        qCCritical(NS_LOG_PIPEWIRE) << this
                                    << "has a device.id property that does not "
                                       "match any known device IDs. Id:"
                                    << id;
      } else {
        QObject::connect(m_device.get(), &QObject::destroyed, this,
            &Node::onDeviceDestroyed);
      }
    }
  }

  if (m_type.testFlags(NodeType::Audio)) {
    m_data = new NodeAudioData(this);
  }
}

QString Node::name() const { return m_name; }

void Node::onDeviceDestroyed() { m_device = nullptr; }

void Node::onCoreSync(quint32 id, qint32 seq) {
  if (id != m_id || seq != m_syncSeq) return;
  m_ready   = true;
  m_syncSeq = 0;
  emit readyChanged();
}

bool Node::shouldUseDevice() const {
  if (!m_device || m_proAudio || m_routeDevice == -1) return false;
  return m_device->hasRouteDevice(m_routeDevice);
}

const struct pw_node_events Node::NODE_EVENTS = {
    .version = PW_VERSION_NODE_EVENTS,
    .info    = &Node::on_info,
    .param   = &Node::on_param};

void Node::on_info(void *data, const struct pw_node_info *info) {
  auto *self = static_cast<Node *>(data);

  if ((info->change_mask & PW_NODE_CHANGE_MASK_PROPS)) {
    string_map_t properties;
    bool         proAudio = false;

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
        if (self->m_data) self->m_data->onDeviceChanged();
      } else {
        qCDebug(NS_LOG_PIPEWIRE)
            << self << "has attached device" << self->m_device.get()
            << "but no card.profile.device property. Using node volume "
               "control.";
      }
    }

    const spa_dict_item *item = nullptr;
    spa_dict_for_each(item, info->props) {
      properties.insert(
          QString::fromUtf8(item->key), QString::fromUtf8(item->value));
    }

    self->m_properties.clear();
    self->m_properties = std::move(properties);
    emit self->propertiesChanged();

    if (self->m_data) self->m_data->onInfo(info);
    if (!self->m_ready && !self->m_syncSeq) {
      auto core = Globals::instance()->core();
      QObject::connect(core, &Core::synced, self, &Node::onCoreSync);
      self->m_syncSeq = core->sync(self->m_id);
    }
  }
}

void Node::on_param(void *data, int seq, uint32_t id, uint32_t index,
    uint32_t next, const struct spa_pod *param) {
  auto *self = static_cast<Node *>(data);
  if (self->m_data) {
    self->m_data->onSpaParam(id, index, param);
  }
}

QString TrackableNode::name() const { return m_name; }
void    TrackableNode::initProps(const spa_dict *props) {
  TrackableObject::initProps(props);
  if (auto it = m_initProps.find(SPA_KEY_NODE_NAME); it != m_initProps.end()) {
    m_name = it.value();
  }
}
} // namespace ns::pipewire
