#pragma once

#include <qcolor.h>
#include <qjsengine.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlengine.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

#include "helpermacros.h"

namespace ns::configs {
class ColorThemeMetadata : public QObject {
  Q_OBJECT

  AUTO_BINDABLE_DEFAULT(ColorThemeMetadata, QString, name, "Default")
  AUTO_BINDABLE_DEFAULT(ColorThemeMetadata, QString, author, "Anonymous")
  AUTO_BINDABLE_DEFAULT(ColorThemeMetadata, QString, version, "1.0")

public:
  explicit ColorThemeMetadata(QObject *parent = nullptr) : QObject(parent) {}
};

class ColorData : public QObject {
  Q_OBJECT

#define C(Name, Value) AUTO_BINDABLE_DEFAULT(ColorData, QColor, Name, Value)
#include "colors.def"
#undef C

public:
  explicit ColorData(QObject *parent = nullptr) : QObject(parent) {}
};

class Colors : public QObject {
  Q_OBJECT
  AUTO_MEYERS_SINGLETON(Colors)

  Q_PROPERTY(ns::configs::ColorThemeMetadata *metadata READ metadata CONSTANT)
  Q_PROPERTY(ns::configs::ColorData *current READ current NOTIFY currentChanged)

public:
  [[nodiscard]] ColorThemeMetadata *metadata();
  [[nodiscard]] ColorData          *current();

signals:
  void currentChanged();

private:
  explicit Colors(QObject *parent = nullptr);

  ColorThemeMetadata m_metadata{this};
  ColorData          m_theme{this};
  ColorData         *m_themePreview = nullptr;

  Q_OBJECT_BINDABLE_PROPERTY(
      Colors, bool, b_isPreviewing, &Colors::currentChanged)
};
} // namespace ns::configs
