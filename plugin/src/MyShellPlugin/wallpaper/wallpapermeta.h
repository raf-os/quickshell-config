#pragma once

#include "wallpapercommon.h"

#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace ns::wallpaper {
class WallpaperMeta : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(QString path READ path NOTIFY pathChanged)
  /// Time before switching to next wallpaper (in seconds)
  /// Should be between -1 and 2147483.
  /// Any positive number lower than 5 will be considered to be 5.
  ///
  /// -1 means the wallpaper won't change.
  Q_PROPERTY(int interval READ interval NOTIFY intervalChanged)
  Q_PROPERTY(ns::wallpaper::WallpaperFillMode::Enum fillMode READ fillMode
                 NOTIFY fillModeChanged)
  Q_PROPERTY(int width READ width NOTIFY widthChanged)
  Q_PROPERTY(int height READ height NOTIFY heightChanged)
  Q_PROPERTY(QString url READ url NOTIFY urlChanged)

public:
  explicit WallpaperMeta(QString  path,
                         QObject *parent = nullptr);

  [[nodiscard]] QString                 path() const { return m_path; }
  [[nodiscard]] int                     interval() const { return m_interval; }
  [[nodiscard]] WallpaperFillMode::Enum fillMode() const { return m_fillmode; }

  void setInterval(const int &value);
  void setFillMode(const QString &value);
  void setFillMode(const WallpaperFillMode::Enum &fillMode);

  [[nodiscard]] int width() const { return m_width; }
  [[nodiscard]] int height() const { return m_height; }

  [[nodiscard]] QString url() const { return m_url; }

signals:
  void pathChanged();
  void intervalChanged();
  void fillModeChanged();
  void widthChanged();
  void heightChanged();
  void urlChanged();

private:
  QString                 m_path;
  int                     m_interval = -1;
  WallpaperFillMode::Enum m_fillmode = WallpaperFillMode::Fit;
  QString                 m_url;

  int m_width  = 1;
  int m_height = 1;
};
} // namespace ns::wallpaper
