#include "notificationserver.h"
#include "dbus_notifications.h"
#include "notification.h"

#include <qcontainerfwd.h>
#include <qdbusconnection.h>
#include <qdbusservicewatcher.h>
#include <qlogging.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qqmlengine.h>
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
  // m_serviceWatcher.addWatchedService("org.freedesktop.Notifications");
  m_serviceWatcher.setConnection(bus);

  // NotificationServer::tryRegister();
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

void NotificationServer::closeConnection() {
  auto bus = QDBusConnection::sessionBus();
  auto succ = bus.unregisterService("org.freedesktop.Notifications");

  if (succ) {
    qCInfo(logNSNotifications) << "Unregistered notification server with dbus.";
  } else {
    qCWarning(logNSNotifications)
        << "Unable to unregister notification server.";
  }
}

void NotificationServer::setIsActive(const bool &value) {
  if (value == m_isActive)
    return;

  m_isActive = value;
  emit isActiveChanged();

  if (m_isActive) {
    m_serviceWatcher.addWatchedService("org.freedesktop.Notifications");

    NotificationServer::tryRegister();
  } else {
    m_serviceWatcher.removeWatchedService("org.freedesktop.Notifications");

    NotificationServer::closeConnection();
  }
}

void NotificationServer::closeAllNotifications() {
  for (auto it = m_notificationsMap.begin(); it != m_notificationsMap.end();
       ++it) {
    it.value()->dismiss();
  }
}

void NotificationServer::onServiceUnregistered(const QString & /*unused*/) {
  NotificationServer::tryRegister();
}

void NotificationServer::resetServerState() {
  m_model.resetState();
  for (auto it = m_notificationsMap.begin(); it != m_notificationsMap.end();
       ++it) {
    it.value()->deleteLater();
  }
  m_notificationsMap.clear();

  emit modelChanged();
}

void NotificationServer::deleteNotification(
    Notification *notification, NotificationCloseReason::Enum reason) {
  if (!m_notificationsMap.contains(notification->id()))
    return;

  emit notification->closed(reason);

  m_model.removeNotification(notification);
  m_notificationsMap.remove(notification->id());

  emit NotificationClosed(notification->id(), reason);
}

void NotificationServer::CloseNotification(uint id) {
  auto *notification = this->m_notificationsMap.value(id);

  if (notification) {
    deleteNotification(notification, NotificationCloseReason::Dismissed);
  }
}

QStringList NotificationServer::GetCapabilities() const {
  auto capabilities = QStringList();

  capabilities.append("persistence");
  capabilities.append("body");

  return capabilities;
}

QString NotificationServer::GetServerInformation(QString &vendor,
                                                 QString &version,
                                                 QString &specVersion) {
  vendor = "nightshell";
  version = "1.0";
  specVersion = "1.3";
  return "Nightshell Notifications";
}

uint NotificationServer::Notify(const QString &appName, uint replacesId,
                                const QString &appIcon, const QString &summary,
                                const QString &body, const QStringList &actions,
                                const QVariantMap &hints, int expireTimeout) {
  auto *notification =
      replacesId == 0 ? nullptr : this->m_notificationsMap.value(replacesId);
  bool isUpdate = notification != nullptr;

  if (!notification) {
    notification = new Notification(m_curId++, this);
    QQmlEngine::setObjectOwnership(notification, QQmlEngine::CppOwnership);
  }

  notification->updateProperties(appName, appIcon, summary, body, actions,
                                 hints, expireTimeout);

  if (!isUpdate) {
    emit this->notification(notification);
    m_notificationsMap.insert(notification->id(), notification);
    m_model.insertNotification(notification);
  }

  return notification->id();
}
} // namespace notifications
} // namespace ns
