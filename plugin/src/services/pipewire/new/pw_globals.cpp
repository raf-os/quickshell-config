#include "pw_globals.h"

#include <memory>

#include <pipewire/context.h>
#include <pipewire/core.h>
#include <pipewire/loop.h>
#include <pipewire/pipewire.h>
#include <qdir.h>
#include <qfilesystemwatcher.h>
#include <qhashfunctions.h>
#include <qloggingcategory.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qsocketnotifier.h>
#include <qtenvironmentvariables.h>
#include <unistd.h>

#include "helpermacros.h"
#include "ns_pipewire_debug.h"
#include "pw_core.h"
#include "pw_defaults.h"
#include "pw_registry.h"

namespace ns::pipewire {
static QString resolveRuntimeDir() {
  auto runtimeDir = qEnvironmentVariable("PIPEWIRE_RUNTIME_DIR");
  if (runtimeDir.isEmpty()) {
    runtimeDir = qEnvironmentVariable("XDG_RUNTIME_DIR");
  }

  if (runtimeDir.isEmpty()) {
    static __uid_t uid = getuid();
    runtimeDir         = QString("/run/user/%1").arg(uid);
  }

  return runtimeDir;
}

AUTO_MEYERS_SINGLETON_IMPL(Globals)

Globals::Globals(QObject *parent) : QObject(parent) {
  pw_init(nullptr, nullptr);

  m_runtimeDir = resolveRuntimeDir();

  qCDebug(NS_LOG_PIPEWIRE) << "runtime dir resolved to" << m_runtimeDir;

  m_core     = new Core(this);
  m_registry = new Registry(m_core, this);
  m_defaults = new DefaultTracker(this);

  QObject::connect(m_core, &Core::fatalError, this, &Globals::queueFatalError);

  if (!tryConnect(false)) {
    beginReconnect();
  }
}

void Globals::queueFatalError() {
  if (m_fatalErrorQueued) return;
  m_fatalErrorQueued = true;
  QMetaObject::invokeMethod(this, &Globals::onFatalError, Qt::QueuedConnection);
}

void Globals::onFatalError() {
  m_fatalErrorQueued = false;

  m_defaults->reset();
  m_registry->reset();
  m_core->shutdown();

  beginReconnect();
}

bool Globals::tryConnect(bool retry) {
  if (m_core->isValid()) return true;

  if (!m_core->init(retry)) {
    return false;
  }

  qCInfo(NS_LOG_PIPEWIRE) << "Connection estabilished.";
  stopSocketWatcher();

  m_registry->init();
  return true;
}

void Globals::beginReconnect() {
  if (m_core->isValid()) {
    stopSocketWatcher();
    return;
  }

  if (!qEnvironmentVariableIsEmpty("PIPEWIRE_REMOTE")) return;
  if (m_runtimeDir.isEmpty()) {
    qCWarning(NS_LOG_PIPEWIRE) << "Unable to watch runtime dir for pipewire "
                                  "reconnects, as it is empty.";
    return;
  }

  startSocketWatcher();
  tryConnect(true);
}

void Globals::startSocketWatcher() {
  if (m_socketWatcher) return;
  if (!qEnvironmentVariableIsEmpty("PIPEWIRE_REMOTE")) return;

  auto dir = QDir(m_runtimeDir);
  if (!dir.exists()) {
    qCWarning(NS_LOG_PIPEWIRE)
        << "Unable to setup a socket watcher: invalid runtime dir:"
        << m_runtimeDir;
    return;
  }

  m_socketWatcher = new QFileSystemWatcher(this);
  m_socketWatcher->addPath(m_runtimeDir);

  QObject::connect(m_socketWatcher, &QFileSystemWatcher::directoryChanged, this,
      &Globals::onRuntimeDirChanged);
}

void Globals::stopSocketWatcher() {
  if (!m_socketWatcher) return;

  QObject::disconnect(m_socketWatcher, nullptr, this, nullptr);
  m_socketWatcher->deleteLater();
  m_socketWatcher = nullptr;
}

void Globals::onRuntimeDirChanged(const QString & /*unused*/) {
  if (m_core->isValid()) {
    stopSocketWatcher();
    return;
  }

  tryConnect(true);
}

Core           *Globals::core() { return m_core; }
Registry       *Globals::registry() { return m_registry; }
DefaultTracker *Globals::defaults() { return m_defaults; }
} // namespace ns::pipewire
