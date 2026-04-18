#pragma once

#include <qcolor.h>
#include <qdir.h>
#include <qhashfunctions.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

#include "propertymacros.h"

// TODO: Write a code generation python script, this will get tedious to do by
// hand

namespace myqmlplugin {
namespace configs {
class ColorConfigMetadata : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  AUTO_PROP_DEFAULT(QString, name, "Default")
  AUTO_PROP_DEFAULT(QString, author, "Anonymous")
  AUTO_PROP_DEFAULT(QString, version, "1.0")

public:
  explicit ColorConfigMetadata(QObject *parent = nullptr) : QObject(parent) {}
};

class ColorConfigColors : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  AUTO_PROP_DEFAULT(QColor, base0, "#1b1510")
  AUTO_PROP_DEFAULT(QColor, base, "#25201d")
  AUTO_PROP_DEFAULT(QColor, base2, "#322d28")
  AUTO_PROP_DEFAULT(QColor, base3, "#4a3d34")
  AUTO_PROP_DEFAULT(QColor, base4, "#776156")
  AUTO_PROP_DEFAULT(QColor, base5, "#9c877d")

  AUTO_PROP_DEFAULT(QColor, baseContent, "#d9d3cd")
  AUTO_PROP_DEFAULT(QColor, baseContentMuted, "#847e78")

  AUTO_PROP_DEFAULT(QColor, primary, "#e25016")
  AUTO_PROP_DEFAULT(QColor, primary5, "#f4b17a")

  AUTO_PROP_DEFAULT(QColor, destructive, "#e32e35")
  AUTO_PROP_DEFAULT(QColor, destructiveHover, "#ed5c4b")

  AUTO_PROP_DEFAULT(QColor, emphasisFavorite, "#efe302")

public:
  explicit ColorConfigColors(QObject *parent = nullptr) : QObject(parent) {}
};
} // namespace configs
} // namespace myqmlplugin

namespace myqmlplugin {
class ColorService : public QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(configs::ColorConfigMetadata *metadata READ metadata NOTIFY
                 metadataChanged)

  Q_PROPERTY(
      configs::ColorConfigColors *colors READ colors NOTIFY colorsChanged)

  Q_PROPERTY(QString configPath READ configPath WRITE setConfigPath NOTIFY
                 configPathChanged)

public:
  explicit ColorService(QObject *parent = nullptr);

  [[nodiscard]] configs::ColorConfigMetadata *metadata() const;
  [[nodiscard]] configs::ColorConfigColors *colors() const;

  [[nodiscard]] QString configPath() const;
  void setConfigPath(const QString &path);

  Q_INVOKABLE void loadConfig();
  Q_INVOKABLE void saveConfig();

signals:
  void metadataChanged();
  void configPathChanged();
  void colorsChanged();

private:
  configs::ColorConfigMetadata *m_configMetadata;
  configs::ColorConfigColors *m_colors;
  QString m_configPath = QDir::homePath() + "/.config/myshell/config";
};
} // namespace myqmlplugin
