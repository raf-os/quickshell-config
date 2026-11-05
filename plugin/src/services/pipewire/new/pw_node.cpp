#include "pw_node.h"

#include <cmath>
#include <cstdint>

#include <pipewire/keys.h>
#include <pipewire/node.h>
#include <qhashfunctions.h>
#include <qlist.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qtypes.h>
#include <spa/node/keys.h>
#include <spa/param/props.h>
#include <spa/pod/iter.h>
#include <spa/pod/pod.h>
#include <spa/utils/dict.h>
#include <spa/utils/keys.h>

#include "ns_pipewire_debug.h"
#include "pw_client.h"
#include "pw_common.h"
#include "pw_device.h"
#include "pw_globals.h"
#include "pw_node_data.h"
#include "pw_registry.h"
#include "pw_trackable.h"

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

Node::Node(const TrackedObjectProps &props, QObject *parent)
    : TrackedObject<pw_node>(props, parent) {
  pw_node_add_listener(proxy(), &m_hook.hook, &Node::NODE_EVENTS, this);

  auto registry = Globals::instance()->registry();
  if (props.initProps.contains(SPA_KEY_MEDIA_CLASS)) {
    auto pkey = props.initProps.value(SPA_KEY_MEDIA_CLASS);
    for (auto [key, value] : MEDIA_CLASS_FLAG_MAP.asKeyValueRange()) {
      if (pkey == key) {
        m_type = value;
        break;
      }
    }
  }

  if (props.initProps.contains(SPA_KEY_NODE_NAME)) {
    m_name = props.initProps.value(SPA_KEY_NODE_NAME);
  }

  if (props.initProps.contains(SPA_KEY_NODE_DESCRIPTION)) {
    m_description = props.initProps.value(SPA_KEY_NODE_DESCRIPTION);
  }

  if (props.initProps.contains(PW_KEY_CLIENT_ID)) {
    bool       ok  = false;
    const auto cid = props.initProps.value(PW_KEY_CLIENT_ID).toInt(&ok);
    if (ok) {
      m_client = registry->getClientById(cid);
      if (m_client) {
        QObject::connect(m_client.get(), &Client::appDataChanged, this,
            &Node::clientAppDataChanged);
      } else {
        qCWarning(NS_LOG_PIPEWIRE)
            << "Could not find client ID" << cid << "for node" << this;
      }
    } else {
      qCWarning(NS_LOG_PIPEWIRE)
          << "Unable to parse client ID for node" << this;
    }
  }

  if (props.initProps.contains(PW_KEY_NODE_NICK)) {
    m_nick = props.initProps.value(PW_KEY_NODE_NICK);
  }

  if (props.initProps.contains(PW_KEY_MEDIA_CATEGORY)) {
    auto nodeCategory = props.initProps.value(PW_KEY_MEDIA_CATEGORY);
    if (nodeCategory == "Monitor" || nodeCategory == "Manager") {
      m_isMonitor = true;
    }
  }

  if (props.initProps.contains(PW_KEY_OBJECT_SERIAL)) {
    auto ok     = false;
    auto serial = props.initProps.value(PW_KEY_OBJECT_SERIAL);
    auto value  = serial.toULongLong(&ok);
    if (!ok) {
      qCWarning(NS_LOG_PIPEWIRE)
          << this << "has invalid object.serial property:" << serial;
    } else {
      m_objectSerial = value;
    }
  }

  if (props.initProps.contains(PW_KEY_DEVICE_ID)) {
    auto ok    = false;
    auto devId = props.initProps.value(PW_KEY_DEVICE_ID);
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

Node::~Node() {
  m_hook.remove();
  if (m_data) {
    delete m_data;
  }
}

Device *Node::getDevice() { return m_device.get(); }
qint32  Node::routeDevice() const { return m_routeDevice; }

INodeData *Node::data() { return m_data; }

QString            Node::name() const { return m_name; }
bool               Node::ready() const { return m_ready; }
Node::string_map_t Node::properties() const { return m_properties; }
NodeState::Enum    Node::state() const { return m_state; }

QString Node::clientName() const {
  if (!m_client) return QString();
  return m_client->name();
}
QString Node::clientIconName() const {
  if (!m_client) return QString();
  return m_client->iconName();
}
QString Node::clientBinary() const {
  if (!m_client) return QString();
  return m_client->binary();
}

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

  if ((info->change_mask & PW_NODE_CHANGE_MASK_STATE)) {
    auto state = static_cast<NodeState::Enum>(info->state);
    if (state != self->m_state) {
      self->m_state = state;
      emit self->stateChanged();
    }
  }

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
    self->m_properties = properties;
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

QString         TrackableNode::name() const { return m_name; }
quint32         TrackableNode::id() const { return m_id; }
NodeType::Flags TrackableNode::type() const { return m_type; }

void TrackableNode::initProps(const spa_dict *props) {
  TrackableObject::initProps(props);
  if (auto it = m_initProps.find(SPA_KEY_NODE_NAME); it != m_initProps.end()) {
    m_name = it.value();
  }

  if (m_initProps.contains(SPA_KEY_MEDIA_CLASS)) {
    auto pkey = m_initProps.value(SPA_KEY_MEDIA_CLASS);
    for (auto [key, value] : MEDIA_CLASS_FLAG_MAP.asKeyValueRange()) {
      if (pkey == key) {
        m_type = value;
        break;
      }
    }
  }
}
} // namespace ns::pipewire
