#include "dbusserver.h"

#include <qdbusconnection.h>
#include <qloggingcategory.h>
#include <qobject.h>

#include "ns_ipc_server.h"
#include "wallpapermanager.h"

namespace ns::ipc::server {
Q_LOGGING_CATEGORY(logNSIpc,
                   "nightshell.ipc")

IPCServer::IPCServer(QObject *parent)
    : QObject(parent),
      m_wallpaperManager(wallpaper::WallpaperManager::instance()) {
  new NightshellIpcServer(this);

  auto bus = QDBusConnection::sessionBus();

  if (!bus.isConnected()) {
    qCWarning(logNSIpc)
        << "Unable to connect to DBus. IPC functionality will not work.";
    return;
  }

  if (!bus.registerService("org.nightshell.socket") ||
      !bus.registerObject("/org/nightshell/socket", this)) {
    qCWarning(logNSIpc) << "Unable to register IPC server with DBus. Is "
                           "another instance already running?";
    return;
  }

  qCInfo(logNSIpc) << "Registered IPC server with DBus.";

  setupWallpaperManagerConnections();
}

void IPCServer::setupWallpaperManagerConnections() {
  if (!m_wallpaperManager) return;

  QObject::connect(m_wallpaperManager,
                   &wallpaper::WallpaperManager::currentChanged,
                   this,
                   &IPCServer::onWallpaperChanged);
}

void IPCServer::onWallpaperChanged() {
  if (!m_wallpaperManager) return;

  auto meta = m_wallpaperManager->current();

  if (!meta) {
    emit WallpaperChanged("");
    return;
  } else {
    const auto path = meta->path();

    emit WallpaperChanged(path);
    return;
  }
}

// DBUS SLOTS
bool IPCServer::AppendWallpaper(const QString &path,
                                const QString &fillMode,
                                const int     &duration) {
  return false;
}

bool IPCServer::AppendWallpaperDialog(const QString &path) {
  emit WallpaperAppendDialogRequested(path);
  return false;
}

bool IPCServer::ChangeWallpaper(const QString &path,
                                const QString &fillMode) {
  if (!m_wallpaperManager) return false;

  m_wallpaperManager->forceSingleWallpaper(path, fillMode);
  return true;
}

bool IPCServer::NextWallpaper() {
  if (!m_wallpaperManager) return false;

  const auto next = m_wallpaperManager->moveForward();

  if (next) return true;
  else return false;
}

bool IPCServer::ToggleLauncher() {
  emit launcherToggleRequested();
  return true;
}

bool IPCServer::OpenLauncher() {
  emit launcherOpenRequested();
  return true;
}

bool IPCServer::ToggleTaskSwitcher() {
  emit taskSwitcherToggleRequested();
  return true;
}
// DBUS SLOTS
} // namespace ns::ipc::server
