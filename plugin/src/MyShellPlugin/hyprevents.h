#pragma once

#include <qlocalsocket.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
namespace myqmlplugin {
class HyprEvents : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

public:
  explicit HyprEvents(QObject *parent = nullptr);
  ~HyprEvents();

  [[nodiscard]] QString socketPath();
  void connectSocket();
  void disconnectSocket();

private slots:
  void onReadyRead();
  void onError(QLocalSocket::LocalSocketError error);

private:
  QLocalSocket *m_socket = nullptr;
  QString m_buffer;
};
} // namespace myqmlplugin
