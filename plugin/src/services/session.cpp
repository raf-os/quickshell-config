#include "session.h"

#include <qdbusabstractinterface.h>
#include <qdbusargument.h>
#include <qdbusconnection.h>
#include <qdbuserror.h>
#include <qdbusextratypes.h>
#include <qdbusmessage.h>
#include <qdbuspendingcall.h>
#include <qdbuspendingreply.h>
#include <qhashfunctions.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qtenvironmentvariables.h>
#include <qtypes.h>
#include <unistd.h>

#include "dbusutils.h"
#include "helpermacros.h"
#include "login1_manager_interface.h"
#include "login1_session_interface.h"
#include "ns_services_debug.h"

static void failed_to_perform_no_dbus_connection() {
  qCWarning(ns::services::NS_LOG_SERVICES)
      << "Failed performing action - DBus interface is not connected.";
}

static bool can_perform_operation(const QString &response) {
  return (response == "yes" || response == "challenge");
}

static constexpr int UPTIME_FETCH_INTERVAL = 150000;

namespace ns::services {
SystemdSession::SystemdSession(const QString &path, QObject *parent)
    : QObject(parent) {
  auto bus = QDBusConnection::systemBus();

  if (!bus.isConnected()) {
    qCWarning(NS_LOG_SERVICES)
        << "SystemdSession: bus is not connected. This should not be possible.";
    return;
  }

  m_interface = new org::freedesktop::login1::Session(
      "org.freedesktop.login1", path, bus, this);

  if (!m_interface->isValid()) {
    qCWarning(NS_LOG_SERVICES)
        << "SystemdSession: unable to create interface for path" << path;
    return;
  }

  m_timer.setInterval(UPTIME_FETCH_INTERVAL);
  m_timer.setSingleShot(true);
  QObject::connect(
      &m_timer, &QTimer::timeout, this, &SystemdSession::fetchUptime);
  fetchUptime();
}

bool SystemdSession::isValid() const {
  if (!m_interface) return false;
  return m_interface->isValid();
}

void SystemdSession::fetchUptime() {
  if (m_isFetching) return;

  auto msg = QDBusMessage::createMethodCall(m_interface->service(),
      m_interface->path(), "org.freedesktop.DBus.Properties", "Get");
  msg << m_interface->interface() << "TimestampMonotonic";

  auto call    = m_interface->connection().asyncCall(msg);
  auto watcher = new QDBusPendingCallWatcher(call, this);

  QObject::connect(watcher, &QDBusPendingCallWatcher::finished, this,
      [this](QDBusPendingCallWatcher *w) {
        QDBusPendingReply<QVariant> reply = *w;
        if (!reply.isError()) {
          b_uptime = qdbus_cast<qulonglong>(reply.value());
        } else {
          qCDebug(NS_LOG_SERVICES) << reply.error();
        }
        m_timer.start();
        w->deleteLater();
      });
}

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

  b_sid        = qEnvironmentVariable("XDG_SESSION_ID");
  auto call    = m_interface->GetSession(b_sid.value());
  auto watcher = new QDBusPendingCallWatcher(call, this);
  dbus::asyncCallback(
      call,
      [this](QDBusObjectPath path) { this->onSessionObjectFetched(path); },
      this);
}

void SessionManager::onSessionObjectFetched(const QDBusObjectPath &path) {
  m_sessionInfo = new SystemdSession(path.path(), this);
  b_uptimeMinutes.setBinding([this] {
    return (m_sessionInfo->bindableUptime().value()) / (1000 * 60);
  });
  b_uptimeHour.setBinding([this] { return b_uptimeMinutes.value() / 60; });
  QObject::connect(m_sessionInfo, &QObject::destroyed, this, [this] {
    b_uptimeMinutes.takeBinding();
    b_uptimeHour.takeBinding();
  });
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
