#pragma once

#include <qcolor.h>
#include <qdir.h>
#include <qfilesystemwatcher.h>
#include <qhashfunctions.h>
#include <qlist.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

#include "cserializable.h"
#include "iconfigserializer.h"
#include "paths.h"
#include "propertymacros.h"

namespace myqmlplugin {
namespace configs {
class ColorConfigMetadata : public myqmlplugin::configs::CSerializable {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  AUTO_PROP_DEFAULT(QString, name, "Default")
  AUTO_PROP_DEFAULT(QString, author, "Anonymous")
  AUTO_PROP_DEFAULT(QString, version, "1.0")

public:
  explicit ColorConfigMetadata(QObject *parent = nullptr)
      : CSerializable(parent) {}
};

class ColorConfigColors : public myqmlplugin::configs::CSerializable {
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

  AUTO_PROP_DEFAULT(QColor, primary0, "#b9370c")
  AUTO_PROP_DEFAULT(QColor, primary, "#e25016")
  AUTO_PROP_DEFAULT(QColor, primary2, "#e67634")
  AUTO_PROP_DEFAULT(QColor, primary3, "#f48743")
  AUTO_PROP_DEFAULT(QColor, primary4, "#e39e59")
  AUTO_PROP_DEFAULT(QColor, primary5, "#f4b17a")

  AUTO_PROP_DEFAULT(QColor, destructive, "#e32e35")
  AUTO_PROP_DEFAULT(QColor, destructiveHover, "#ed5c4b")

  AUTO_PROP_DEFAULT(QColor, emphasisFavorite, "#efe302")

public:
  explicit ColorConfigColors(QObject *parent = nullptr)
      : CSerializable(parent) {}
};
} // namespace configs
} // namespace myqmlplugin

namespace myqmlplugin {
class Colors : public QObject, public configs::IConfigSerializer {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON
  Q_INTERFACES(myqmlplugin::configs::IConfigSerializer)

  Q_PROPERTY(myqmlplugin::configs::ColorConfigMetadata *metadata READ metadata
                 NOTIFY metadataChanged)

  Q_PROPERTY(myqmlplugin::configs::ColorConfigColors *colors READ colors NOTIFY
                 colorsChanged)

  Q_PROPERTY(QString configPath READ configPath WRITE setConfigPath NOTIFY
                 configPathChanged)
  Q_PROPERTY(QString themeName READ themeName WRITE setThemeName NOTIFY
                 themeNameChanged)
  Q_PROPERTY(QList<QString> themeList READ themeList NOTIFY themeListChanged)

public:
  explicit Colors(QObject *parent = nullptr);

  [[nodiscard]] configs::ColorConfigMetadata *metadata() const;
  [[nodiscard]] configs::ColorConfigColors *colors() const;

  [[nodiscard]] QString configPath() const;
  void setConfigPath(const QString &path);

  [[nodiscard]] QString themeName() const;
  void setThemeName(const QString &name);

  [[nodiscard]] QList<QString> themeList() const;

  Q_INVOKABLE void loadConfig();
  Q_INVOKABLE void saveConfig();

  void commitSave() override;

signals:
  void metadataChanged();
  void configPathChanged();
  void colorsChanged();
  void themeNameChanged();
  void themeListChanged();

private:
  configs::ColorConfigMetadata *m_configMetadata;
  configs::ColorConfigColors *m_colors;
  QFileSystemWatcher *m_fileWatcher;

  QList<QString> m_themeDb;

  QString m_configPath = utils::Paths::instance()->config();
  QString m_themeName = "default";

  QDir checkConfigPath();
  void writeConfigToPath(const QString &name);
  void buildThemeDb();
  void attachFileWatcher();
  void resetConfigs();

  void onFileWatcherChanged(const QString &path);
};

} // namespace myqmlplugin
