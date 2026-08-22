#pragma once

#include <qcontainerfwd.h>
#include <qdbuscontext.h>
#include <qdbusservicewatcher.h>
#include <qlist.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qtypes.h>
namespace ns::systemtray {
class StatusNotifierWatcher : public QObject, protected QDBusContext {
  Q_OBJECT

public:
  static StatusNotifierWatcher *instance();

  [[nodiscard]] bool        isRegistered() const;
  [[nodiscard]] qint32      protocolVersion() const { return 0; }
  [[nodiscard]] QStringList registeredItems() const;

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

private slots:
  void onServiceUnregistered(const QString &service);

private:
  explicit StatusNotifierWatcher(QObject *parent = nullptr);

  void    tryRegister();
  QString normalizeItemId(const QString &item);

  bool                m_isRegistered = false;
  QDBusServiceWatcher m_serviceWatcher;
  QStringList         m_items;
  QStringList         m_hosts;
};
} // namespace ns::systemtray
