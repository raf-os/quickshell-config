#pragma once

#include <qjsengine.h>
#include <qobject.h>
#include <qpointer.h>
#include <qqmlengine.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

#include "helpermacros.h"
#include "wallpapermanager.h"

namespace ns::ipc::server {
class IPCServer : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  AUTO_MEYERS_SINGLETON_QML_DECL(IPCServer)

public:
  Q_INVOKABLE void setup() {
    // This is simply here to instantiate this from QML
  }

public slots:
  bool AppendWallpaper(
      const QString &path, const QString &fillMode, const int &duration);
  bool AppendWallpaperDialog(const QString &path);
  bool ChangeWallpaper(const QString &path, const QString &fillMode);
  bool NextWallpaper();
  bool ToggleLauncher();
  bool OpenLauncher();
  bool ToggleTaskSwitcher();
  bool SessionLock();

private slots:
  void setupWallpaperManagerConnections();
  void onWallpaperChanged();

signals:
  // DBUS Signals
  void WallpaperChanged(QString path);
  // DBUS Signals
  void WallpaperAppendDialogRequested(const QString &path);

  void launcherToggleRequested();
  void launcherOpenRequested();
  void taskSwitcherToggleRequested();
  void sessionLockRequested();

private:
  explicit IPCServer(QObject *parent = nullptr);

  QPointer<wallpaper::WallpaperManager> m_wallpaperManager;
};
} // namespace ns::ipc::server
