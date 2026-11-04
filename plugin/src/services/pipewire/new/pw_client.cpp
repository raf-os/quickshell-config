#include "pw_client.h"

#include <pipewire/client.h>
#include <pipewire/keys.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qstringview.h>
#include <qtypes.h>
#include <spa/utils/dict.h>

#include "ns_pipewire_debug.h"
#include "pw_trackable.h"

namespace ns::pipewire {
Client::Client(const TrackedObjectProps &props, QObject *parent)
    : TrackedObject<pw_client>(props, parent) {
  pw_client_add_listener(proxy(), &m_hook.hook, &Client::CLIENT_EVENTS, this);

  m_appData = {.name = props.initProps.value(PW_KEY_APP_NAME),
      .binary        = props.initProps.value(PW_KEY_APP_PROCESS_BINARY),
      .iconName      = props.initProps.value(PW_KEY_APP_ICON_NAME)};
}

Client::~Client() { m_hook.remove(); }

QString Client::name() const { return m_appData.name; }
QString Client::binary() const { return m_appData.binary; }
QString Client::iconName() const { return m_appData.iconName; }

const struct pw_client_events Client::CLIENT_EVENTS = {
    .version     = PW_VERSION_CLIENT_EVENTS,
    .info        = &Client::on_info,
    .permissions = &Client::on_permissions};

void Client::on_info(void *data, const struct pw_client_info *info) {
  auto *self = static_cast<Client *>(data);

  if (info->change_mask & PW_CLIENT_CHANGE_MASK_PROPS) {
    bool appDataChanged = false;
    if (const auto *clientName =
            spa_dict_lookup(info->props, PW_KEY_CLIENT_NAME))
    {
      self->m_appData.name = clientName;
      appDataChanged       = true;
    }
    if (const auto *processBinary =
            spa_dict_lookup(info->props, PW_KEY_APP_PROCESS_BINARY))
    {
      self->m_appData.binary = processBinary;
      appDataChanged         = true;
    }
    if (const auto *appIconName =
            spa_dict_lookup(info->props, PW_KEY_APP_ICON_NAME))
    {
      self->m_appData.iconName = appIconName;
      appDataChanged           = true;
    }

    if (appDataChanged) {
      emit self->appDataChanged();
    }
  }
}
void Client::on_permissions(void *data, uint32_t index, uint32_t n_permissions,
    const struct pw_permission *permissions) {
  // auto *self = static_cast<Client *>(data);
}

quint32 TrackableClient::id() const { return m_id; }
} // namespace ns::pipewire
