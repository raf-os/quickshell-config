#pragma once

#include <qdbusabstractinterface.h>
#include <qdbusconnection.h>
#include <qdbuspendingreply.h>
#include <qhashfunctions.h>
#include <qlist.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qvariant.h>

#include "helpermacros.h"

class OrgFreedesktopLogin1ManagerInterface : public QDBusAbstractInterface {
  Q_OBJECT

public:
  static inline const char *staticInterfaceName() {
    return "org.freedesktop.login1.Manager";
  }

  explicit OrgFreedesktopLogin1ManagerInterface(
      const QDBusConnection &connection, QObject *parent = nullptr);
  ~OrgFreedesktopLogin1ManagerInterface();

public slots:
  inline QDBusPendingReply<> PowerOff(bool interactive) {
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(interactive);
    return asyncCallWithArgumentList("PowerOff", argumentList);
  }

  inline QDBusPendingReply<> Reboot(bool interactive) {
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(interactive);
    return asyncCallWithArgumentList("Reboot", argumentList);
  }

  inline QDBusPendingReply<> Suspend(bool interactive) {
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(interactive);
    return asyncCallWithArgumentList("Suspend", argumentList);
  }

  inline QDBusPendingReply<> Hibernate(bool interactive) {
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(interactive);
    return asyncCallWithArgumentList("Hibernate", argumentList);
  }

#define CAN(Action)                                                            \
  inline QDBusPendingReply<QString> Can##Action() {                            \
    return asyncCall("Can" #Action);                                           \
  }
  CAN(PowerOff)
  CAN(Reboot)
  CAN(Halt)
  CAN(Suspend)
  CAN(Hibernate)
  CAN(Sleep)
#undef CAN

signals:
  void PrepareForShutdown(bool start);
  void PrepareForSleep(bool start);
};

namespace org::freedesktop {
using Login1Manager = ::OrgFreedesktopLogin1ManagerInterface;
};

namespace ns::services {
class SessionManager : public QObject {
  Q_OBJECT

  AUTO_MEYERS_SINGLETON_DECL(SessionManager)

private:
  explicit SessionManager(QObject *parent = nullptr);

  org::freedesktop::Login1Manager *m_interface = nullptr;
};
} // namespace ns::services
