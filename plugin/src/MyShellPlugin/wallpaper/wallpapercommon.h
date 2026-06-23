#pragma once

#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qtypes.h>

namespace ns::wallpaper {
class WallpaperFillMode : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

public:
  enum Enum : quint8 { Fit = 0, Fill = 1, Stretch = 2 };
  Q_ENUM(Enum)

  Q_INVOKABLE static QString toString(WallpaperFillMode::Enum value) {
    switch (value) {
    case WallpaperFillMode::Fit:
      return "Fit";
    case WallpaperFillMode::Fill:
      return "Fill";
    default:
      return "Stretch";
    }
  }

  Q_INVOKABLE static ns::wallpaper::WallpaperFillMode::Enum
  fromString(const QString &value) {
    auto lowered = value.trimmed().toLower();
    if (lowered == "fit")
      return WallpaperFillMode::Fit;
    else if (lowered == "fill")
      return WallpaperFillMode::Fill;
    else if (lowered == "stretch")
      return WallpaperFillMode::Stretch;
    else
      return WallpaperFillMode::Fit;
  }
};
} // namespace ns::wallpaper
