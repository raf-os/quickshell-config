#include "pw_link.h"

#include <cstring>
#include <utility>

#include <pipewire/keys.h>
#include <pipewire/link.h>
#include <qdebug.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qproperty.h>
#include <qtypes.h>
#include <spa/utils/dict.h>

#include "ns_services_pipewire_debug.h"
#include "pw_bindable.h"

namespace ns::services::pipewire {
Link::Link(PwBindableProps props) : PwBindableBase(std::move(props)) {}

const pw_link_events Link::LINK_EVENTS = {
    .version = PW_VERSION_LINK, .info = &Link::link_on_info};

void Link::link_on_info(void *data, const struct pw_link_info *info) {
  auto self = static_cast<Link *>(data);

  self->m_outputNode = info->output_node_id;
  self->m_inputNode  = info->input_node_id;

  if (info->change_mask & PW_LINK_CHANGE_MASK_STATE) {
    self->b_state = static_cast<Link::State>(info->state);
  }
}

void Link::bindHooks() {
  pw_link_add_listener(
      m_interface.proxy(), &m_listener.hook, &Link::LINK_EVENTS, this);
}

void Link::unbindHooks() {
  m_listener.remove();
  b_state = State::Unlinked;
}

quint32 Link::outputNode() const { return m_outputNode; }
quint32 Link::inputNode() const { return m_inputNode; }

void Link::initProps(const spa_dict *props) {
  const spa_dict_item *item = nullptr;

  auto assignIf = [&item, this](const char *key, quint32 *memberVariable) {
    if (strcmp(item->key, key) != 0) return false;
    QString str{item->value};
    bool    ok    = false;
    auto    value = str.toInt(&ok);
    if (ok) {
      *memberVariable = value;
    } else {
      qCWarning(NS_LOG_PIPEWIRE) << "Unable to parse" << key << "for" << this;
    }

    return true;
  };

  spa_dict_for_each(item, props) {
    if (!assignIf(PW_KEY_LINK_INPUT_NODE, &m_inputNode)) {
      assignIf(PW_KEY_LINK_OUTPUT_NODE, &m_outputNode);
    }
  }
}

QBindable<Link::State> Link::bindableState() const { return &b_state; }

QDebug operator<<(QDebug debug, const Link *link) {
  if (!link) {
    debug << "Link(0x0)";
  } else {
    QDebugStateSaver saver{debug};
    debug.nospace() << "Link(" << static_cast<const void *>(link) << ")["
                    << link->outputNode() << "->" << link->inputNode()
                    << ", state=" << link->bindableState().value() << "]";
  }
  return debug;
}
} // namespace ns::services::pipewire
