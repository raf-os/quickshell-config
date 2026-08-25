#include "statusnotifierhost.h"

#include <qcontainerfwd.h>
#include <qdbusconnection.h>
#include <qdbuserror.h>
#include <qlist.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <unistd.h>

#include "dbus_watcher_interface.h"
#include "dbusutils.h"
#include "statusnotifieritem.h"
#include "statusnotifierwatcher.h"

namespace ns::systemtray {
Q_LOGGING_CATEGORY(logNSStatusNotifierHost, "ns.systemtray.StatusNotifierHost")

StatusNotifierHost::StatusNotifierHost(QObject *parent) : QObject(parent) {
  StatusNotifierWatcher::instance();

  auto bus = QDBusConnection::sessionBus();

  if (!bus.isConnected()) {
    qCWarning(logNSStatusNotifierHost) << "Unable to connect do DBus.";
    return;
  }

  m_hostId =
      QString("org.kde.StatusNotifierHost-%1").arg(QString::number(getpid()));

  auto succ = bus.registerService(m_hostId);

  if (!succ) {
    qCWarning(logNSStatusNotifierHost)
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

  m_watcher = new QDBusStatusNotifierWatcher(
      "org.kde.StatusNotifierWatcher", "/StatusNotifierWatcher", bus, this);

  QObject::connect(m_watcher,
                   &QDBusStatusNotifierWatcher::StatusNotifierItemRegistered,
                   this,
                   &StatusNotifierHost::onItemRegistered);

  QObject::connect(m_watcher,
                   &QDBusStatusNotifierWatcher::StatusNotifierItemUnregistered,
                   this,
                   &StatusNotifierHost::onItemUnregistered);

  if (!m_watcher->isValid()) {
    qCWarning(logNSStatusNotifierHost)
        << "Unable to find active StatusNotifierWatcher.";
    return;
  }

  this->connectToWatcher();
}

StatusNotifierHost *StatusNotifierHost::instance() {
  static auto s_instance = new StatusNotifierHost();
  return s_instance;
}

void StatusNotifierHost::connectToWatcher() {
  m_watcher->RegisterStatusNotifierHost(m_hostId);

  dbus::asyncReadProperty<QStringList>(
      *m_watcher,
      "RegisteredStatusNotifierItems",
      [this](QStringList value, QDBusError error) {
        if (error.isValid()) {
          qCWarning(logNSStatusNotifierHost).noquote()
              << "Error reading \"RegisteredStatusNotifierItems\" property of "
                 "wacher"
              << m_watcher->service();
          qCWarning(logNSStatusNotifierHost) << error;
        } else {
          qCDebug(logNSStatusNotifierHost)
              << "Registering pre-existing status notifier items from watcher:"
              << value;

          for (auto &item : value) {
            this->onItemRegistered(item);
          }
        }
      });
}

void StatusNotifierHost::onWatcherRegistered() { connectToWatcher(); }
void StatusNotifierHost::onWatcherUnregistered() {
  for (auto [service, item] : m_items.asKeyValueRange()) {
    emit itemUnregistered(item);
    item->deleteLater();
  }

  m_items.clear();
}

void StatusNotifierHost::onItemRegistered(const QString &item) {
  if (m_items.contains(item)) {
    qCDebug(logNSStatusNotifierHost)
        << "Skipping duplicate StatusNotifierItem" << item;
    return;
  }

  qCDebug(logNSStatusNotifierHost)
      << "Received new StatusNotifierItem:" << item;
  auto *nItem = new StatusNotifierItem(item, this);
  if (!nItem->isValid()) {
    qCWarning(logNSStatusNotifierHost)
        << "Unable to connect to StatusNotifierItem at" << nItem;
    nItem->deleteLater();
    return;
  }

  m_items.insert(item, nItem);
  QObject::connect(nItem,
                   &StatusNotifierItem::ready,
                   this,
                   &StatusNotifierHost::onItemReady);
  emit itemRegistered(nItem);
}

void StatusNotifierHost::onItemUnregistered(const QString &item) {
  if (auto *nItem = m_items.value(item)) {
    m_items.remove(item);
    emit itemUnregistered(nItem);
    nItem->deleteLater();
    qCDebug(logNSStatusNotifierHost)
        << "Unregistered StatusNotifierItem" << item << "from host.";
  } else {
    qCWarning(logNSStatusNotifierHost)
        << "Received signal to unregister StatusNotifierItem" << item
        << ", yet said item was not being tracked.";
  }
}

void StatusNotifierHost::onItemReady() {
  if (auto *item = qobject_cast<StatusNotifierItem *>(sender())) {
    emit itemReady(item);
  }
}

QList<StatusNotifierItem *> StatusNotifierHost::items() const {
  auto items = m_items.values();
  items.removeIf([](StatusNotifierItem *item) { return !item->isReady(); });
  return items;
}
} // namespace ns::systemtray
