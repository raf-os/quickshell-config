#include "statusnotifierwatcher.h"

#include <algorithm>

#include <qcontainerfwd.h>
#include <qdbusconnection.h>
#include <qdbuscontext.h>
#include <qdbusmetatype.h>
#include <qdbusservicewatcher.h>
#include <qloggingcategory.h>
#include <qobject.h>

#include "dbus_watcher.h"

namespace ns::systemtray {
Q_LOGGING_CATEGORY(logNSStatusNotifierWatcher,
                   "ns.systemtray.StatusNotifierWatcher")

StatusNotifierWatcher::StatusNotifierWatcher(QObject *parent)
    : QObject(parent) {
  new StatusNotifierWatcherAdaptor(this);

  auto bus = QDBusConnection::sessionBus();

  if (!bus.isConnected()) {
    qCWarning(logNSStatusNotifierWatcher) << "Unable to connect to DBus.";
    return;
  }

  if (!bus.registerObject("/StatusNotifierWatcher", this)) {
    qCWarning(logNSStatusNotifierWatcher)
        << "Unable to register StatusNotifierWatcher object with DBus. Service "
           "will not work.";
    return;
  }

  QObject::connect(&m_serviceWatcher,
                   &QDBusServiceWatcher::serviceUnregistered,
                   this,
                   &StatusNotifierWatcher::onServiceUnregistered);

  m_serviceWatcher.setWatchMode(QDBusServiceWatcher::WatchForUnregistration);
  m_serviceWatcher.addWatchedService("org.kde.StatusNotifierWatcher");
  m_serviceWatcher.setConnection(bus);

  tryRegister();
}

StatusNotifierWatcher *StatusNotifierWatcher::instance() {
  static auto s_instance = new StatusNotifierWatcher();
  return s_instance;
}

bool StatusNotifierWatcher::isRegistered() const { return m_isRegistered; }

void StatusNotifierWatcher::tryRegister() {
  auto bus  = QDBusConnection::sessionBus();
  auto succ = bus.registerService("org.kde.StatusNotifierWatcher");

  if (!succ) {
    qCWarning(logNSStatusNotifierWatcher)
        << "Unable to register watcher. Is one already running?";
    return;
  }

  qCDebug(logNSStatusNotifierWatcher)
      << "Registered service watcher successfully.";
  m_isRegistered = true;
}

void StatusNotifierWatcher::onServiceUnregistered(const QString &service) {
  if (service == "org.kde.StatusNotifierWatcher") {
    qCDebug(logNSStatusNotifierWatcher)
        << "Current service unregistered, attempting registration...";
    tryRegister();
    return;
  }

  auto it = std::ranges::find_if(
      m_items.begin(), m_items.end(), [&service](const QString &item) {
        if (item.startsWith(service)) {
          return true;
        }
        return false;
      });

  if (it != m_items.end()) {
    qCDebug(logNSStatusNotifierWatcher)
        << "Unregistered StatusNotifierItem" << *it;
    emit StatusNotifierItemUnregistered(*it);
    m_items.erase(it);
  } else if (m_hosts.removeAll(service) != 0) {
    qCDebug(logNSStatusNotifierWatcher)
        << "Unregistered StatusNotifierHost" << service;
    emit StatusNotifierHostUnregistered();
  } else {
    qCWarning(logNSStatusNotifierWatcher)
        << "Received unregistration event, but provided service" << service
        << "was not previously registered.";
  }

  m_serviceWatcher.removeWatchedService(service);
}

QStringList StatusNotifierWatcher::registeredItems() const { return m_items; }

void StatusNotifierWatcher::RegisterStatusNotifierHost(const QString &host) {
  if (m_hosts.contains(host)) {
    qCDebug(logNSStatusNotifierWatcher)
        << "Attempted registering host" << host
        << ", but it was already being tracked. Skipping.";
    return;
  }

  if (!QDBusConnection::sessionBus().interface()->serviceOwner(host).isValid())
  {
    qCWarning(logNSStatusNotifierWatcher)
        << "Attempted registering invalid host" << host << ", skipping.";
    return;
  }

  m_serviceWatcher.addWatchedService(host);
  m_hosts.append(host);
  qCDebug(logNSStatusNotifierWatcher)
      << "Sucecssfully registered StatusNotifierHost" << host;
  emit StatusNotifierHostRegistered();
}

void StatusNotifierWatcher::RegisterStatusNotifierItem(const QString &item) {
  auto normalizedItem = normalizeItemId(item);

  if (m_items.contains(normalizedItem)) {
    qCDebug(logNSStatusNotifierWatcher)
        << "Attempted registering duplicate StatusNotifierItem"
        << normalizedItem << ", skipping.";
    return;
  }

  auto service = normalizedItem.split("/").at(0);

  if (!QDBusConnection::sessionBus()
           .interface()
           ->serviceOwner(service)
           .isValid())
  {
    qCWarning(logNSStatusNotifierWatcher)
        << "Attempted registering invalid StatusNotifierItem" << normalizedItem
        << ", skipping.";
    return;
  }

  m_serviceWatcher.addWatchedService(service);
  m_items.append(normalizedItem);

  qCDebug(logNSStatusNotifierWatcher)
      << "Successfully registered StatusNotifierItem" << normalizedItem;

  emit StatusNotifierItemRegistered(normalizedItem);
}

QString StatusNotifierWatcher::normalizeItemId(const QString &item) {
  QString normalized;
  if (item.startsWith("/")) {
    normalized = QDBusContext::message().service() + item;
  } else {
    normalized = item;
  }

  if (!normalized.contains("/")) {
    normalized += "/StatusNotifierItem";
  }

  return normalized;
}
} // namespace ns::systemtray
