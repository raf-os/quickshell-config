#pragma once

#include <qdbusservicewatcher.h>
#include <qhash.h>
#include <qlist.h>
#include <qobject.h>
#include <qtmetamacros.h>

#include "dbus_watcher_interface.h"
#include "statusnotifieritem.h"

namespace ns::systemtray {
// On the Dbus side, this will only register a unique service
class StatusNotifierHost : public QObject {
  Q_OBJECT

public:
  static StatusNotifierHost *instance();

  void connectToWatcher();

  [[nodiscard]] QList<StatusNotifierItem *> items() const;

signals:
  void itemRegistered(StatusNotifierItem *item);
  void itemReady(StatusNotifierItem *item);
  void itemUnregistered(StatusNotifierItem *item);

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

  explicit StatusNotifierHost(QObject *parent = nullptr);
};
} // namespace ns::systemtray
