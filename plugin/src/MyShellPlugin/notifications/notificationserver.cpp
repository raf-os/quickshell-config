#include "notificationserver.h"
#include "dbus_notifications.h"

#include <qcontainerfwd.h>
#include <qdbusconnection.h>
#include <qdbusservicewatcher.h>
#include <qlogging.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qtypes.h>

namespace ns {
namespace notifications {
Q_LOGGING_CATEGORY(logNSNotifications, "nightshell.notifications")

NotificationServer::NotificationServer(QObject *parent) : QObject(parent) {
  new DBusNotificationServer(this);

  qCInfo(logNSNotifications) << "Starting notification server...";

  auto bus = QDBusConnection::sessionBus();

  if (!bus.isConnected()) {
    qCWarning(logNSNotifications) << "Unable to connect to DBus.";
    return;
  }

  if (!bus.registerObject("/org/freedesktop/Notifications", this)) {
    qCWarning(logNSNotifications)
        << "Unable to register notification server with DBus.";
    return;
  }

  QObject::connect(&m_serviceWatcher, &QDBusServiceWatcher::serviceUnregistered,
                   this, &NotificationServer::onServiceUnregistered);

  m_serviceWatcher.setWatchMode(QDBusServiceWatcher::WatchForUnregistration);
  m_serviceWatcher.addWatchedService("org.freedesktop.Notifications");
  m_serviceWatcher.setConnection(bus);

  NotificationServer::tryRegister();
}

void NotificationServer::tryRegister() {
  auto bus = QDBusConnection::sessionBus();
  auto succ = bus.registerService("org.freedesktop.Notifications");

  if (succ) {
    qCInfo(logNSNotifications) << "Registered notification server with dbus.";
  } else {
    qCWarning(logNSNotifications) << "Unable to register notification server "
                                     "at org.freedesktop.Notifications, is a "
                                     "notification daemon already running?";
    qCWarning(logNSNotifications) << "Service will attempt to register again "
                                     "once the current one is unregistered.";
  }
}

void NotificationServer::onServiceUnregistered(const QString & /*unused*/) {
  NotificationServer::tryRegister();
}

void NotificationServer::CloseNotification(uint id) {}

QStringList NotificationServer::GetCapabilities() const {
  auto capabilities = QStringList();
  return capabilities;
}

QString NotificationServer::GetServerInformation(QString &vendor,
                                                 QString &version,
                                                 QString &specVersion) {
  return "";
}

uint NotificationServer::Notify(const QString &appName, uint replacesId,
                                const QString &appIcon, const QString &summary,
                                const QString &body, const QStringList &actions,
                                const QVariantMap &hints, int expireTimeout) {
  return 1;
}
} // namespace notifications
} // namespace ns
