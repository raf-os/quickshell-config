#include "session.h"

#include <qdbusabstractinterface.h>
#include <qdbusconnection.h>
#include <qdbuspendingcall.h>
#include <qdbuspendingreply.h>
#include <qhashfunctions.h>
#include <qloggingcategory.h>
#include <qobject.h>

#include "dbusutils.h"
#include "helpermacros.h"
#include "login1_manager_interface.h"
#include "ns_services_debug.h"

static void failed_to_perform_no_dbus_connection() {
  qCWarning(ns::services::NS_LOG_SERVICES)
      << "Failed performing action - DBus interface is not connected.";
}

static bool can_perform_operation(const QString &response) {
  return (response == "yes" || response == "challenge");
}

namespace ns::services {
AUTO_MEYERS_SINGLETON_QML_IMPL(SessionManager)

SessionManager::SessionManager(QObject *parent) : QObject(parent) {
  auto bus = QDBusConnection::systemBus();

  if (!bus.isConnected()) {
    qCWarning(NS_LOG_SERVICES) << "SessionManager: bus is not connected.";
    return;
  }

  m_interface = new org::freedesktop::login1::Manager(
      "org.freedesktop.login1", "/org/freedesktop/login1", bus, this);

  if (!m_interface->isValid()) {
    qCWarning(NS_LOG_SERVICES)
        << "SessionManager: unable to create dbus interface";
    return;
  }
}

void SessionManager::wrapped_check_call(
    CanCheckFn checkFn, OperationFn onSuccess) {
  if (b_isPending.value()) return;
  if (!m_interface->isValid()) {
    return failed_to_perform_no_dbus_connection();
  }
  auto call = (m_interface->*checkFn)();
  dbus::asyncCallback(
      call,
      [this, onSuccess](const QString &resp) {
        if (can_perform_operation(resp)) {
          auto op = (m_interface->*onSuccess)(true);
          dbus::asyncCallback(op, nullptr, this);
        } else {
          qCWarning(NS_LOG_SERVICES)
              << "SessionManager: User is not allowed to perform action.";
        }
        b_isPending = false;
      },
      this);
}

void SessionManager::shutdown() {
  wrapped_check_call(&interface_t::CanPowerOff, &interface_t::PowerOff);
}

void SessionManager::reboot() {
  wrapped_check_call(&interface_t::CanReboot, &interface_t::Reboot);
}
} // namespace ns::services
