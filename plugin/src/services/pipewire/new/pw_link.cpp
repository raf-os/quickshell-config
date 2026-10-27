#include "pw_link.h"

#include <pipewire/keys.h>
#include <pipewire/link.h>
#include <qloggingcategory.h>
#include <qproperty.h>
#include <qtypes.h>

#include "ns_pipewire_debug.h"

namespace ns::pipewire {
QBindable<Link::State> Link::bindableState() const { return &b_state; }
quint32                Link::outputNode() const { return m_outputNode; }
quint32                Link::inputNode() const { return m_inputNode; }

void Link::bindHooks() {
  pw_link_add_listener(proxy(), &m_hook.hook, &Link::LINK_EVENTS, this);
}

void Link::unbindHooks() {
  m_hook.remove();
  b_state = State::Unlinked;
}

void Link::initProps(const string_map_t &props) {
  auto assignIf = [&props, this](const QString &key, quint32 *memberVariable) {
    if (!props.contains(key)) return;
    bool ok    = false;
    auto value = key.toInt(&ok);
    if (ok) {
      *memberVariable = value;
    } else {
      qCWarning(NS_LOG_PIPEWIRE) << "Unable to parse" << key << "for" << this;
    }
  };

  assignIf(PW_KEY_LINK_INPUT_NODE, &m_inputNode);
  assignIf(PW_KEY_LINK_OUTPUT_NODE, &m_outputNode);
}

const struct pw_link_events Link::LINK_EVENTS = {
    .version = PW_VERSION_LINK_EVENTS, .info = &Link::on_info};

void Link::on_info(void *data, const struct pw_link_info *info) {
  auto self = static_cast<Link *>(data);

  self->m_outputNode = info->output_node_id;
  self->m_inputNode  = info->input_node_id;

  if (info->change_mask & PW_LINK_CHANGE_MASK_STATE) {
    self->b_state = static_cast<Link::State>(info->state);
  }
}
} // namespace ns::pipewire
