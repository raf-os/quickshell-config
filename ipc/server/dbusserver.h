#pragma once

#include <qjsengine.h>
#include <qobject.h>
#include <qpointer.h>
#include <qqmlengine.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

#include "wallpapermanager.h"

namespace ns::ipc::server {
class IPCServer : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

public:
  static IPCServer *instance() {
    static IPCServer *s_instance = new IPCServer();
    return s_instance;
  }

  static IPCServer *create(QQmlEngine *qmlEngine,
                           QJSEngine * /* unused */) {
    auto i = instance();
    if (qmlEngine) qmlEngine->setObjectOwnership(i, QQmlEngine::CppOwnership);
    return i;
  }

  Q_INVOKABLE void setup() {
    // This is simply here to instantiate this from QML
  }

public slots:
  bool AppendWallpaper(const QString &path,
                       const QString &fillMode,
                       const int     &duration);
  bool AppendWallpaperDialog(const QString &path);
  bool ChangeWallpaper(const QString &path,
                       const QString &fillMode);
  bool NextWallpaper();
  bool ToggleLauncher();
  bool OpenLauncher();
  bool ToggleTaskSwitcher();

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

private:
  explicit IPCServer(QObject *parent = nullptr);

  QPointer<wallpaper::WallpaperManager> m_wallpaperManager;
};
} // namespace ns::ipc::server
