#pragma once

#include <qlocalsocket.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace ns::hyprland {
class HyprEvents : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(bool isConnected READ isConnected NOTIFY isConnectedChanged)

public:
  explicit HyprEvents(QObject *parent = nullptr);
  ~HyprEvents();

  [[nodiscard]] bool isConnected() const;

  [[nodiscard]] QString socketPath();
  void                  connectSocket();
  void                  disconnectSocket();

private slots:
  void onReadyRead();
  void onError(QLocalSocket::LocalSocketError error);

signals:
  void configReloaded();
  void bellRang();
  void keyboardLayoutChanged(QString keyboardName,
                             QString layoutName);
  void isConnectedChanged();

private:
  bool          m_isConnected;
  QLocalSocket *m_socket = nullptr;
  QString       m_buffer;

  void dispatchEvent(const QString &event,
                     const QString &data);
};
} // namespace ns::hyprland
