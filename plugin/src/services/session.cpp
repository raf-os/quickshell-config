#include "session.h"

#include <qdbusabstractinterface.h>
#include <qdbusconnection.h>
#include <qlogging.h>
#include <qobject.h>

#include "helpermacros.h"

OrgFreedesktopLogin1ManagerInterface::OrgFreedesktopLogin1ManagerInterface(
    const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface("org.freedesktop.login1",
          "/org/freedesktop/login1", staticInterfaceName(), connection,
          parent) {}

OrgFreedesktopLogin1ManagerInterface::~OrgFreedesktopLogin1ManagerInterface() {}

namespace ns::services {
AUTO_MEYERS_SINGLETON_IMPL(SessionManager)

SessionManager::SessionManager(QObject *parent) : QObject(parent) {
  auto bus = QDBusConnection::sessionBus();

  if (!bus.isConnected()) {
    qWarning() << "SessionManager: bus is not connected.";
    return;
  }

  m_interface = new org::freedesktop::Login1Manager(bus, this);

  if (!m_interface->isValid()) {
    qWarning() << "SessionManager: unable to create dbus interface";
    return;
  }
}
} // namespace ns::services
