#include "hyprctl.h"

#include <utility>

#include <QLocalSocket>

#include "ns_hyprland_debug.h"

namespace ns::wayland::hyprland {
Hyprctl::Hyprctl(QString socketPath, QObject *parent)
    : QObject(parent), m_socketPath(std::move(socketPath)) {}

void Hyprctl::call(QObject *context, const QByteArray &request,
                   const std::function<void(bool, QByteArray)> &callback) {
  if (m_socketPath.isEmpty()) return;

  auto requestSocket = new QLocalSocket(this);

  auto onConnected = [this, context, request, requestSocket, callback]() {
    auto responseCallback = [requestSocket, callback]() {
      if (callback) {
        auto response = requestSocket->readAll();
        callback(true, std::move(response));
      }
      delete requestSocket;
    };

    QObject::connect(requestSocket, &QLocalSocket::readyRead, context,
                     responseCallback);

    requestSocket->write(request);
    requestSocket->flush();
  };

  auto onError = [=](QLocalSocket::LocalSocketError error) {
    qCWarning(NS_LOG_HYPRLAND)
        << "Error making hyprland request:" << error << ", request:" << request;
    requestSocket->deleteLater();
    if (callback) {
      callback(false, {});
    }
  };

  QObject::connect(requestSocket, &QLocalSocket::connected, this, onConnected);
  QObject::connect(requestSocket, &QLocalSocket::errorOccurred, this, onError);

  requestSocket->connectToServer(m_socketPath);
}

void Hyprctl::dispatch(const QString &request) {
  auto escaped = request;
  escaped.replace("'", "\\'");
  auto rchar         = QString("dispatch %0").arg(escaped).toUtf8();
  auto requestSocket = new QLocalSocket(this);

  call(this, request.toUtf8());
}

void Hyprctl::reloadConfig() { call(this, "reload"); }
} // namespace ns::wayland::hyprland
