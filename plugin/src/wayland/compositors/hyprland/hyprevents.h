#pragma once

#include <memory>

#include <QByteArray>
#include <QByteArrayView>
#include <QObject>
#include <QProperty>
#include <QString>
#include <QtTypes>

namespace ns::wayland::hyprland {
class HyprEventsPrivate;

class HyprEvents : public QObject {
  Q_OBJECT

public:
  explicit HyprEvents(const QString &socketPath, QObject *parent = nullptr);
  ~HyprEvents() override;

  [[nodiscard]] QBindable<bool> bindableIsConnected() const;
  [[nodiscard]] bool            isConnected() const;

  void connect();
  void disconnect();

signals:
  void isConnectedChanged();
  void configReloaded();
  void bellRang();
  void keyboardLayoutChanged(QString keyboardName, QString layoutName);
  void activeWindowChanged(quint64 address);
  void windowOpened(quint64 address, QByteArray workspaceAddress);
  void workspaceCreated();
  void workspaceDestroyed(QByteArray workspaceAddress);
  void workspaceMoved(QByteArray workspaceAddress, QString monitorName);
  void workspaceRenamed(QByteArray workspaceAddress, QString workspaceName);
  void userWorkspaceChanged(QByteArray workspaceAddress);
  void windowMoved(quint64 address, QByteArray workspaceAddress);
  void windowFullscreen();
  void monitorFocused(QString monitorName, QByteArray workspaceAddress);
  void windowUrgent(quint64 address);

private slots:
  void onReadyRead();

private:
  void parseBuffer();
  void dispatchEvent(QByteArrayView event, QByteArrayView data);

  std::unique_ptr<HyprEventsPrivate> d;

  QByteArray m_buffer;

  Q_OBJECT_BINDABLE_PROPERTY(HyprEvents, bool, b_isConnected,
                             &HyprEvents::isConnectedChanged)
};
} // namespace ns::wayland::hyprland
