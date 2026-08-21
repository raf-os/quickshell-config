#include "statusnotifierhost.h"

#include <qcontainerfwd.h>
#include <qdbusconnection.h>
#include <qdbuserror.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <unistd.h>

#include "dbus_watcher_interface.h"
#include "dbusutils.h"
#include "statusnotifierwatcher.h"

namespace ns::systemtray {
Q_LOGGING_CATEGORY(logNSStatusNotifier, "ns.systemtray")

StatusNotifierHost::StatusNotifierHost(QObject *parent) : QObject(parent) {
  StatusNotifierWatcher::instance();

  auto bus = QDBusConnection::sessionBus();

  if (!bus.isConnected()) {
    qCWarning(logNSStatusNotifier) << "Unable to connect do DBus.";
    return;
  }

  m_hostId =
      QString("org.kde.StatusNotifierHost-%1").arg(QString::number(getpid()));

  auto succ = bus.registerService(m_hostId);

  if (!succ) {
    qCWarning(logNSStatusNotifier)
        << "Unable to register StatusNotifierHost with DBus.";
    return;
  }

  QObject::connect(&m_serviceWatcher,
                   &QDBusServiceWatcher::serviceRegistered,
                   this,
                   &StatusNotifierHost::onWatcherRegistered);

  QObject::connect(&m_serviceWatcher,
                   &QDBusServiceWatcher::serviceUnregistered,
                   this,
                   &StatusNotifierHost::onWatcherUnregistered);

  m_serviceWatcher.addWatchedService("org.kde.StatusNotifierWatcher");
  m_serviceWatcher.setConnection(bus);

  m_watcher = new QDbusStatusNotifierWatcher(
      "org.kde.StatusNotifierWatcher", "/StatusNotifierWatcher", bus, this);

  QObject::connect(m_watcher,
                   &QDbusStatusNotifierWatcher::StatusNotifierItemRegistered,
                   this,
                   &StatusNotifierHost::onItemRegistered);

  QObject::connect(m_watcher,
                   &QDbusStatusNotifierWatcher::StatusNotifierItemUnregistered,
                   this,
                   &StatusNotifierHost::onItemUnregistered);

  if (!m_watcher->isValid()) {
    qCWarning(logNSStatusNotifier)
        << "Unable to find active StatusNotifierWatcher.";
    return;
  }

  this->connectToWatcher();
}

void StatusNotifierHost::connectToWatcher() {
  m_watcher->RegisterStatusNotifierHost(m_hostId);

  dbus::asyncReadProperty<QStringList>(
      *m_watcher,
      "RegisteredStatusNotifierItems",
      [this](QStringList value, QDBusError error) {
        if (error.isValid()) {
          qCWarning(logNSStatusNotifier).noquote()
              << "Error reading \"RegisteredStatusNotifierItems\" property of "
                 "wacher"
              << m_watcher->service();
          qCWarning(logNSStatusNotifier) << error;
        } else {
          qCDebug(logNSStatusNotifier)
              << "Registering pre-existing status notifier items from watcher:"
              << value;

          for (auto &item : value) {
            this->onItemRegistered(item);
          }
        }
      });
}
} // namespace ns::systemtray
