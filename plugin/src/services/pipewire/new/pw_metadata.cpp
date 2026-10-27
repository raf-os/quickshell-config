#include "pw_metadata.h"

#include <pipewire/core.h>
#include <pipewire/extensions/metadata.h>
#include <pipewire/permission.h>
#include <qloggingcategory.h>

#include "ns_pipewire_debug.h"

namespace ns::pipewire {
const struct pw_metadata_events Metadata::METADATA_EVENTS = {
    .version = PW_VERSION_METADATA_EVENTS, .property = Metadata::on_property};

int Metadata::on_property(void *data, uint32_t subject, const char *key,
    const char *type, const char *value) {
  auto *self = static_cast<Metadata *>(data);
  emit  self->propertyChanged(key, type, value);
  return 0;
}

void Metadata::bindHooks() {
  pw_metadata_add_listener(
      proxy(), &m_hook.hook, &Metadata::METADATA_EVENTS, this);
}

void Metadata::unbindHooks() { m_hook.remove(); }

void Metadata::initProps(const string_map_t &props) {
  if (props.contains(PW_KEY_METADATA_NAME)) {
    m_name = props.value(PW_KEY_METADATA_NAME);
  }
}

bool Metadata::hasSetPermissions() const {
  return (m_perms & (PW_PERM_W | PW_PERM_X)) == (PW_PERM_W | PW_PERM_X);
}

void Metadata::setProperty(
    const char *key, const char *type, const char *value) {
  if (!proxy()) {
    qCCritical(NS_LOG_PIPEWIRE)
        << "Attempted changing unbound property of" << this;
    return;
  }

  if (!hasSetPermissions()) {
    qCCritical(NS_LOG_PIPEWIRE) << "Attempted changing property of" << this
                                << "without proper read/write permissions.";
  }

  pw_metadata_set_property(proxy(), PW_ID_CORE, key, type, value);
}
} // namespace ns::pipewire
