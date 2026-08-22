#pragma once

#include <qdbusservicewatcher.h>
#include <qhash.h>
#include <qobject.h>
#include <qtmetamacros.h>

#include "dbus_watcher_interface.h"
#include "statusnotifieritem.h"

namespace ns::systemtray {
// On the Dbus side, this will only register a unique service
class StatusNotifierHost : public QObject {
  Q_OBJECT

public:
  explicit StatusNotifierHost(QObject *parent = nullptr);

  void connectToWatcher();

private slots:
  void onWatcherRegistered();
  void onWatcherUnregistered();
  void onItemRegistered(const QString &item);
  void onItemUnregistered(const QString &item);
  void onItemReady();

private:
  QString                              m_hostId;
  QDBusServiceWatcher                  m_serviceWatcher;
  QDBusStatusNotifierWatcher          *m_watcher = nullptr;
  QHash<QString, StatusNotifierItem *> m_items;
};
} // namespace ns::systemtray
