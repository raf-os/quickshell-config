#pragma once

#include <functional>

#include <qdbusabstractinterface.h>
#include <qdbusconnection.h>
#include <qdbuspendingreply.h>
#include <qhashfunctions.h>
#include <qlist.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qvariant.h>

#include "helpermacros.h"

class OrgFreedesktopLogin1ManagerInterface;

namespace ns::services {
class SessionManager : public QObject {
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
#undef B

public:
  Q_INVOKABLE void shutdown();
  Q_INVOKABLE void reboot();

private:
  explicit SessionManager(QObject *parent = nullptr);

  OrgFreedesktopLogin1ManagerInterface *m_interface = nullptr;

  void wrapped_check_call(CanCheckFn checkFn, OperationFn onSuccess);
};
} // namespace ns::services
