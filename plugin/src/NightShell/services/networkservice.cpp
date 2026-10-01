#include "networkservice.h"

#include "helpermacros.h"

namespace ns::services {
NetworkService::NetworkService(QObject *parent) : QObject(parent) {
  m_nmi = new org::freedesktop::NetworkManager("org.freedesktop.NetworkManager",
      "/org/freedesktop/NetworkManager", QDBusConnection::systemBus(), this);

  m_NMState = m_nmi->state();

  QObject::connect(m_nmi, &org::freedesktop::NetworkManager::StateChanged, this,
      &NetworkService::onNetworkStateChanged);
}

AUTO_MEYERS_SINGLETON_QML_IMPL(NetworkService)

void NetworkService::onNetworkStateChanged(uint state) {
  if (m_NMState == state) return;

  m_NMState = state;
  emit connectivityChanged();
}

bool NetworkService::isConnected() const { return m_NMState == 70; }

bool NetworkService::isConnecting() const { return m_NMState == 40; }

bool NetworkService::isLimitedConnection() const { return m_NMState == 60; }
} // namespace ns::services
