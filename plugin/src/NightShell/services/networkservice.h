#pragma once

#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "helpermacros.h"
#include "nm_interface.h"

namespace ns::services {
class NetworkService : public QObject {
  Q_OBJECT
  QML_SINGLETON
  QML_ELEMENT

  AUTO_MEYERS_SINGLETON_QML(NetworkService)

  Q_PROPERTY(bool isConnecting READ isConnecting NOTIFY connectivityChanged)
  Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectivityChanged)
  Q_PROPERTY(bool isLimitedConnection READ isLimitedConnection NOTIFY
          connectivityChanged)
public:
  [[nodiscard]] bool isConnecting() const;
  [[nodiscard]] bool isConnected() const;
  [[nodiscard]] bool isLimitedConnection() const;

signals:
  void connectivityChanged();

private:
  explicit NetworkService(QObject *parent = nullptr);
  org::freedesktop::NetworkManager *m_nmi;
  uint                              m_NMState;

  void onNetworkStateChanged(uint state);
};
} // namespace ns::services
