#include "mpris_service_watcher.h"

#include <qcontainerfwd.h>
#include <qdbusconnection.h>
#include <qdbusconnectioninterface.h>
#include <qdbusservicewatcher.h>
#include <qloggingcategory.h>
#include <qobject.h>

#include "ns_mpris_debug.h"

namespace ns::mpris {
MprisServiceWatcher::MprisServiceWatcher(QObject *parent) : QObject(parent) {
  auto bus = QDBusConnection::sessionBus();

  if (!bus.isConnected()) {
    qCWarning(NS_LOG_MPRIS)
        << "Unable to connect to DBus. Mpris service is unavailable.";
    return;
  }

  QObject::connect(&m_watcher, &QDBusServiceWatcher::serviceRegistered, this,
      &MprisServiceWatcher::onServiceRegistered);
  QObject::connect(&m_watcher, &QDBusServiceWatcher::serviceUnregistered, this,
      &MprisServiceWatcher::onServiceUnregistered);

  m_watcher.setWatchMode(QDBusServiceWatcher::WatchForUnregistration |
                         QDBusServiceWatcher::WatchForRegistration);

  m_watcher.addWatchedService("org.mpris.MediaPlayer2");
  m_watcher.setConnection(bus);

  const QStringList &existingList = bus.interface()->registeredServiceNames();
  for (const QString &service : existingList) {
    if (service.startsWith("org.mpris.MediaPlayer2")) {
      qCDebug(NS_LOG_MPRIS) << "Registering existing service:" << service;
      // TODO: register player
    }
  }
}

void MprisServiceWatcher::onServiceRegistered(const QString &service) {}

void MprisServiceWatcher::onServiceUnregistered(const QString &service) {}
} // namespace ns::mpris
