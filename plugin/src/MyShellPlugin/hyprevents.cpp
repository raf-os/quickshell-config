#include "hyprevents.h"

#include <qdebug.h>
#include <qlocalsocket.h>
#include <qobject.h>
#include <qstring.h>
#include <qtenvironmentvariables.h>

namespace myqmlplugin {
HyprEvents::HyprEvents(QObject *parent) : QObject(parent) {
  m_socket = new QLocalSocket(this);

  QObject::connect(m_socket, &QLocalSocket::readyRead, this,
                   &HyprEvents::onReadyRead);
}

HyprEvents::~HyprEvents() {
  if (m_socket->state() != QLocalSocket::UnconnectedState) {
    m_socket->disconnectFromServer();
    if (m_socket->state() != QLocalSocket::UnconnectedState) {
      m_socket->waitForDisconnected(1000);
    }
  }
}

void HyprEvents::connectSocket() {
  m_socket->connectToServer(socketPath(), QLocalSocket::ReadOnly);
}

void HyprEvents::disconnectSocket() {
  if (m_socket->state() != QLocalSocket::UnconnectedState) {
    m_socket->disconnectFromServer();
  }
}

void HyprEvents::onReadyRead() {
  m_buffer += QString::fromUtf8(m_socket->readAll());

  while (m_buffer.contains('\n')) {
    const int newline = m_buffer.indexOf('\n');
    const QString line = m_buffer.left(newline).trimmed();
    m_buffer = m_buffer.mid(newline + 1);

    if (line.isEmpty())
      continue;
  }
}

QString HyprEvents::socketPath() {
  const QString runtimeDir = qEnvironmentVariable("XDG_RUNTIME_DIR");
  const QString his = qEnvironmentVariable("HYPRLAND_INSTANCE_SIGNATURE");
  return QString("%1/hypr/%2/.socket2.sock").arg(runtimeDir, his);
}

void HyprEvents::onError(QLocalSocket::LocalSocketError error) {
  qWarning() << "myqmlplugin::HyprEvents: Hyprland socket error: "
             << m_socket->errorString();
}
} // namespace myqmlplugin
