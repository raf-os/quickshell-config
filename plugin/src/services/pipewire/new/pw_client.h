#pragma once

#include <cstdint>

#include <pipewire/client.h>
#include <pipewire/permission.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <spa/utils/dict.h>

#include "pw_spa_hook.h"
#include "pw_trackable.h"

namespace ns::pipewire {
struct ClientAppData {
  QString name;
  QString binary;
  QString iconName;
};

class Client : public TrackedObject<pw_client> {
  Q_OBJECT

public:
  explicit Client(const TrackedObjectProps &props, QObject *parent = nullptr);
  ~Client() override;

  [[nodiscard]] QString name() const;
  [[nodiscard]] QString binary() const;
  [[nodiscard]] QString iconName() const;

signals:
  void appDataChanged();

private:
  SpaHook m_hook;

  ClientAppData m_appData;

  static const struct pw_client_events CLIENT_EVENTS;
  static void on_info(void *data, const struct pw_client_info *info);
  static void on_permissions(void *data, uint32_t index, uint32_t n_permissions,
      const struct pw_permission *permissions);
};

class TrackableClient : public TrackableObject<Client, PW_TYPE_INTERFACE_Client,
                            PW_VERSION_CLIENT> {
public:
  explicit TrackableClient(quint32 id, quint32 perms)
      : TrackableObject(id, perms), m_id(id) {}

  [[nodiscard]] quint32 id() const;

private:
  quint32 m_id;
};
} // namespace ns::pipewire
