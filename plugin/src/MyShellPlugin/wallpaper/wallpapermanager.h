#pragma once

#include <qfilesystemwatcher.h>
#include <qjsengine.h>
#include <qlist.h>
#include <qobject.h>
#include <qqmlengine.h>
#include <qqmlintegration.h>
#include <qtimer.h>
#include <qtmetamacros.h>

#include "helpermacros.h"
#include "wallpapermeta.h"

namespace ns::wallpaper {
class WallpaperManager : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  AUTO_MEYERS_SINGLETON_QML_DECL(WallpaperManager)

  Q_PROPERTY(
      ns::wallpaper::WallpaperMeta *current READ current NOTIFY currentChanged)

public:
  struct ParsedJsonObject {
    QString path;
    QString fillMode;
    int     interval;
  };

  [[nodiscard]] WallpaperMeta *current();

  Q_SLOT WallpaperMeta *moveForward();
  Q_SLOT void           triggerParse();

signals:
  void currentChanged();

public slots:
  void forceSingleWallpaper(const QString &path, const QString &fillMode);

private slots:
  void parseConfig();
  void setupTimer();
  void saveConfigToFile();

private:
  explicit WallpaperManager(QObject *parent = nullptr);

  QString                m_configPath;
  QFileSystemWatcher     m_fileWatcher;
  QList<WallpaperMeta *> m_instances;
  QTimer                 m_switchTimer;
  QTimer                 m_fileDebounceTimer;
  int                    m_currentIndex = 0;

  void attachWatcher();
};
} // namespace ns::wallpaper
