#pragma once

#include <qdbusabstractinterface.h>
#include <qdbusconnection.h>
#include <qdbusextratypes.h>
#include <qdbuspendingreply.h>
#include <qhashfunctions.h>
#include <qlist.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qvariant.h>

#include "helpermacros.h"
#include "ns_services_export.h"

class OrgFreedesktopLogin1ManagerInterface;
class OrgFreedesktopLogin1SessionInterface;

namespace ns::services {
class NS_SERVICES_EXPORT SystemdSession : public QObject {
  Q_OBJECT

  using interface_t = OrgFreedesktopLogin1SessionInterface;

public:
  explicit SystemdSession(const QString &path, QObject *parent = nullptr);

  [[nodiscard]] bool               isValid() const;
  [[nodiscard]] QBindable<quint64> bindableUptime() const { return &b_uptime; }

public slots:
  void fetchUptime();

signals:
  void uptimeChanged();

private:
  bool                                  m_isFetching = false;
  OrgFreedesktopLogin1SessionInterface *m_interface  = nullptr;
  QTimer                                m_timer;

  Q_OBJECT_BINDABLE_PROPERTY(
      SystemdSession, quint64, b_uptime, &SystemdSession::uptimeChanged);
};

class NS_SERVICES_EXPORT SessionManager : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  AUTO_MEYERS_SINGLETON_QML_DECL(SessionManager)

  using CanCheckFn = QDBusPendingReply<QString> (
      OrgFreedesktopLogin1ManagerInterface::*)();
  using OperationFn = QDBusPendingReply<> (
      OrgFreedesktopLogin1ManagerInterface::*)(bool);
  using interface_t = OrgFreedesktopLogin1ManagerInterface;

#define B(Type, Name) AUTO_BINDABLE(SessionManager, Type, Name)
  B(bool, isPending)
  B(quint64, uptimeHour)
  B(quint64, uptimeMinutes)
#undef B

public:
  Q_INVOKABLE void shutdown();
  Q_INVOKABLE void reboot();

private slots:
  void onSessionObjectFetched(const QDBusObjectPath &path);

private:
  explicit SessionManager(QObject *parent = nullptr);

  OrgFreedesktopLogin1ManagerInterface *m_interface   = nullptr;
  SystemdSession                       *m_sessionInfo = nullptr;

  void wrapped_check_call(CanCheckFn checkFn, OperationFn onSuccess);

  Q_OBJECT_BINDABLE_PROPERTY(SessionManager, QString, b_sid)
};
} // namespace ns::services
