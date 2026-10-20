#include "pw_metadata.h"

#include <cstdint>

#include <pipewire/core.h>
#include <pipewire/extensions/metadata.h>
#include <pipewire/permission.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qtypes.h>

#include "ns_services_pipewire_debug.h"
#include "pw_bindable.h"

static int pw_metadata_on_property(void *data, uint32_t subject,
    const char *key, const char *type, const char *value) {
  auto self = static_cast<ns::services::pipewire::PwMetadata *>(data);
  emit self->propertyChanged(QString::fromUtf8(key), QString::fromUtf8(type),
      QString::fromUtf8(value));
  return 0;
}

static const struct pw_metadata_events NS_PW_METADATA_EVENTS = {
    .version = PW_VERSION_METADATA_EVENTS, .property = pw_metadata_on_property};

namespace ns::services::pipewire {
PwMetadata::PwMetadata(PwBindableProps props)
    : PwBindableBase(std::move(props)) {}

const QString &PwMetadata::name() const { return m_name; }

void PwMetadata::bindHooks() {
  pw_metadata_add_listener(
      m_interface.proxy(), &m_listener.hook, &NS_PW_METADATA_EVENTS, this);
}

void PwMetadata::unbindHooks() { m_listener.remove(); }

bool PwMetadata::hasSetPermissions() const {
  return (m_perms & (PW_PERM_W | PW_PERM_X)) == (PW_PERM_W | PW_PERM_X);
}

void PwMetadata::setProperty(
    const char *key, const char *type, const char *value) {
  if (!m_interface.proxy()) {
    qCCritical(NS_LOG_PIPEWIRE)
        << "Tried changing property of" << this << "which is unbound.";
    return;
  }

  if (!hasSetPermissions()) {
    qCCritical(NS_LOG_PIPEWIRE) << "Tried changing property of" << this
                                << "without successful permissions.";
  }

  pw_metadata_set_property(m_interface.proxy(), PW_ID_CORE, key, type, value);
}
} // namespace ns::services::pipewire
