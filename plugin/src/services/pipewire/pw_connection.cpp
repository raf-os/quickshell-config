#include "pw_connection.h"

#include <qdir.h>
#include <qfilesystemwatcher.h>
#include <qloggingcategory.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qtenvironmentvariables.h>
#include <unistd.h>

#include "helpermacros.h"
#include "ns_services_pipewire_debug.h"
#include "pwcore.h"

static const QString resolveRuntimeDir() {
  auto runtimeDir = qEnvironmentVariable("PIPEWIRE_RUNTIME_DIR");
  if (runtimeDir.isEmpty()) {
    runtimeDir = qEnvironmentVariable("XDG_RUNTIME_DIR");
  }
  if (runtimeDir.isEmpty()) {
    runtimeDir = QString("/run/user/%1").arg(getuid());
  }
  return runtimeDir;
}

namespace ns::services::pipewire {
AUTO_MEYERS_SINGLETON_IMPL(Connection)

Connection::Connection(QObject *parent) : QObject(parent) {
  m_runtimeDir = resolveRuntimeDir();

  QObject::connect(
      &m_core, &PwCore::fatalError, this, &Connection::queueFatalError);

  if (!tryConnect(false)) {}
}

bool Connection::tryConnect(bool retry) {
  if (m_core.isValid()) return true;

  if (!m_core.init(retry)) {
    return false;
  }

  qCInfo(NS_LOG_PIPEWIRE) << "Connection estabilished.";
  stopSocketWatcher();

  m_registry.init(m_core);
  return true;
}

void Connection::beginReconnect() {
  if (m_core.isValid()) {
    stopSocketWatcher();
    return;
  }

  if (!qEnvironmentVariableIsEmpty("PIPEWIRE_REMOTE")) return;

  if (m_runtimeDir.isEmpty()) {
    return;
  }

  startSocketWatcher();
  tryConnect(true);
}

void Connection::startSocketWatcher() {
  if (m_socketWatcher) return;
  if (!qEnvironmentVariableIsEmpty("PIPEWIRE_REMOTE")) return;

  QDir dir{m_runtimeDir};
  if (!dir.exists()) {
    qCWarning(NS_LOG_PIPEWIRE)
        << "Connection: Missing runtime dir:" << m_runtimeDir;
    return;
  }

  m_socketWatcher = new QFileSystemWatcher(this);
  m_socketWatcher->addPath(m_runtimeDir);

  QObject::connect(m_socketWatcher, &QFileSystemWatcher::directoryChanged, this,
      &Connection::onRuntimeDirChanged);
}

void Connection::stopSocketWatcher() {
  if (!m_socketWatcher) return;

  QObject::disconnect(m_socketWatcher, nullptr, this, nullptr);
  m_socketWatcher->deleteLater();
  m_socketWatcher = nullptr;
}

void Connection::onRuntimeDirChanged(const QString & /*unused*/) {
  if (m_core.isValid()) {
    stopSocketWatcher();
    return;
  }

  tryConnect(true);
}

void Connection::queueFatalError() {
  if (m_fatalErrorQueued) return;

  m_fatalErrorQueued = true;
  QMetaObject::invokeMethod(
      this, &Connection::onFatalError, Qt::QueuedConnection);
}

void Connection::onFatalError() {
  m_fatalErrorQueued = false;

  // TODO: defaults
  m_registry.reset();
  m_core.shutdown();

  beginReconnect();
}
} // namespace ns::services::pipewire
