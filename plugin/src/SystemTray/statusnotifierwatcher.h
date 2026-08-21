#pragma once

#include <qobject.h>
#include <qtmetamacros.h>
namespace ns::systemtray {
class StatusNotifierWatcher : public QObject {
  Q_OBJECT

public:
  static StatusNotifierWatcher *instance();

  // DBUS ADAPTOR
  void RegisterStatusNotifierHost(const QString &host);
  void RegisterStatusNotifierItem(const QString &item);
  // DBUS ADAPTOR

signals:
  // DBUS ADAPTOR
  void StatusNotifierHostRegistered();
  void StatusNotifierHostUnregistered();
  void StatusNotifierItemRegistered(const QString &service);
  void StatusNotifierItemUnregistered(const QString &service);
  // DBUS ADAPTOR

private:
  explicit StatusNotifierWatcher(QObject *parent = nullptr);
};
} // namespace ns::systemtray
