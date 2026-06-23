#include "wallpapermeta.h"
#include "wallpapercommon.h"
#include <qobject.h>

namespace ns::wallpaper {
WallpaperMeta::WallpaperMeta(QString  path,
                             QObject *parent)
    : QObject(parent),
      m_path(path) {};

void WallpaperMeta::setInterval(const int &value) {
  if (value == m_interval)
    return;
  m_interval = value;
  emit intervalChanged();
}

void WallpaperMeta::setFillMode(const WallpaperFillMode::Enum &value) {
  if (m_fillmode == value)
    return;
  m_fillmode = value;
  emit fillModeChanged();
}

void WallpaperMeta::setFillMode(const QString &value) {
  auto i = WallpaperFillMode::fromString(value);
  setFillMode(i);
}
} // namespace ns::wallpaper
