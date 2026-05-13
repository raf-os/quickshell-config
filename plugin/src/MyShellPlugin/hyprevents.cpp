#include "hyprevents.h"

#include <qdebug.h>
#include <qlocalsocket.h>
#include <qlogging.h>
#include <qobject.h>
#include <qstring.h>
#include <qtenvironmentvariables.h>

namespace myqmlplugin {
HyprEvents::HyprEvents(QObject *parent) : QObject(parent) {
  m_socket = new QLocalSocket(this);

  QObject::connect(m_socket, &QLocalSocket::readyRead, this,
                   &HyprEvents::onReadyRead);
  QObject::connect(m_socket, &QLocalSocket::stateChanged, this,
                   [this](const QLocalSocket::LocalSocketState socketState) {
                     if (socketState == QLocalSocket::ConnectedState) {
                       if (m_isConnected == false) {
                         m_isConnected = true;
                         emit isConnectedChanged();
                       }
                     } else {
                       if (m_isConnected) {
                         m_isConnected = false;
                         emit isConnectedChanged();
                       }
                     }
                   });
  QObject::connect(m_socket, &QLocalSocket::errorOccurred, this,
                   &HyprEvents::onError);
}

HyprEvents::~HyprEvents() {
  if (m_socket->state() != QLocalSocket::UnconnectedState) {
    m_socket->disconnectFromServer();
    if (m_socket->state() != QLocalSocket::UnconnectedState) {
      m_socket->waitForDisconnected(1000);
    }
  }
}

bool HyprEvents::isConnected() const { return m_isConnected; }

void HyprEvents::connectSocket() {
  qDebug() << "Connecting to hyprland's socket2...";
  m_socket->connectToServer(socketPath(), QLocalSocket::ReadOnly);
}

void HyprEvents::disconnectSocket() {
  if (m_socket->state() != QLocalSocket::UnconnectedState) {
    qDebug() << "Disconnecting from hyprland's socket2...";
    m_socket->disconnectFromServer();
  }
}

void HyprEvents::onReadyRead() {
  m_buffer += QString::fromUtf8(m_socket->readAll());

  while (m_buffer.contains('\n')) {
    const int newline = m_buffer.indexOf('\n');
    const QString line = m_buffer.first(newline).trimmed();

    // Checking for overflow just to be sure
    if (newline + 1 >= m_buffer.size())
      m_buffer = "";
    else
      m_buffer = m_buffer.sliced(newline + 1);

    if (line.isEmpty())
      continue;

    const int separator = line.indexOf(">>");
    if (separator == -1)
      continue;

    const QString event = line.first(separator).trimmed();
    const QString data = line.sliced(separator + 2).trimmed();

    dispatchEvent(event, data);
  }
}

void HyprEvents::dispatchEvent(const QString &event, const QString &data) {
  if (event == "configreloaded") {
    emit configReloaded();
    return;
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
