#include "pw_node.h"

#include <cstdint>
#include <cstring>
#include <utility>

#include <pipewire/keys.h>
#include <pipewire/node.h>
#include <qhash.h>
#include <qmap.h>
#include <spa/node/keys.h>
#include <spa/pod/pod.h>
#include <spa/utils/dict.h>
#include <spa/utils/keys.h>

#include "pw_bindable.h"
#include "pw_common.h"

static void pw_node_on_info(void *data, const struct pw_node_info *info) {}
static void pw_node_on_param(void *data, int seq, uint32_t id, uint32_t index,
    uint32_t next, const struct spa_pod *param) {}

static const struct pw_node_events NS_PIPEWIRE_NODE_EVENTS = {
    .version = PW_VERSION_NODE_EVENTS,
    .info    = pw_node_on_info,
    .param   = pw_node_on_param};

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

Node::Node(PwBindableProps props) : PwBindableBase(std::move(props)) {}

void Node::bindHooks() {
  pw_node_add_listener(
      m_interface.proxy(), &m_listener.hook, &NS_PIPEWIRE_NODE_EVENTS, this);
}
void Node::unbindHooks() {}

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
}
} // namespace ns::services::pipewire
