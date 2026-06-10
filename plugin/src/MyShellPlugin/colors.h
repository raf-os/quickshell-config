#pragma once

#include <qcolor.h>
#include <qdir.h>
#include <qfilesystemwatcher.h>
#include <qhashfunctions.h>
#include <qjsengine.h>
#include <qlist.h>
#include <qobject.h>
#include <qqmlengine.h>
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

  AUTO_PROP_DEFAULT(QColor, neutral, "#0a0a0a")
  AUTO_PROP_DEFAULT(QColor, neutralContent, "#d9d3cd")

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

  Q_PROPERTY(myqmlplugin::configs::ColorConfigMetadata *metadataPreview READ
                 metadataPreview NOTIFY themePreviewChanged)

  Q_PROPERTY(myqmlplugin::configs::ColorConfigColors *colorsPreview READ
                 colorsPreview NOTIFY themePreviewChanged)

  Q_PROPERTY(QString configPath READ configPath WRITE setConfigPath NOTIFY
                 configPathChanged)
  Q_PROPERTY(QString themeName READ themeName WRITE setThemeName NOTIFY
                 themeNameChanged)
  Q_PROPERTY(QList<QString> themeList READ themeList NOTIFY themeListChanged)
  Q_PROPERTY(bool isPreviewing READ isPreviewing NOTIFY isPreviewingChanged)
  Q_PROPERTY(QString previewThemeName READ previewThemeName NOTIFY
                 previewThemeNameChanged)

public:
  static Colors *instance() {
    static Colors *s_instance = new Colors();
    return s_instance;
  }

  static Colors *create(QQmlEngine *qmlEngine, QJSEngine *) {
    auto inst = instance();
    if (qmlEngine)
      qmlEngine->setObjectOwnership(inst, QJSEngine::CppOwnership);
    return instance();
  }

  struct ColorObjPayload {
    configs::ColorConfigMetadata *metadata = Colors::instance()->metadata();
    configs::ColorConfigColors *colors = Colors::instance()->colors();
  };

  [[nodiscard]] configs::ColorConfigMetadata *metadata() const;
  [[nodiscard]] configs::ColorConfigColors *colors() const;

  [[nodiscard]] configs::ColorConfigMetadata *metadataPreview() const;
  [[nodiscard]] configs::ColorConfigColors *colorsPreview() const;

  [[nodiscard]] QString configPath() const;
  void setConfigPath(const QString &path);

  [[nodiscard]] QString themeName() const;
  void setThemeName(const QString &name);

  [[nodiscard]] QList<QString> themeList() const;

  [[nodiscard]] bool isPreviewing() const;
  [[nodiscard]] QString previewThemeName() const;

  Q_INVOKABLE void earlyLoad() {};
  Q_INVOKABLE void loadConfig();
  Q_INVOKABLE void saveConfig();
  Q_INVOKABLE void loadPreview(const QString &themeName,
                               QObject *handler = nullptr);
  Q_INVOKABLE void closePreview();

  Q_INVOKABLE void commitSave() override;

signals:
  void metadataChanged();
  void configPathChanged();
  void colorsChanged();
  void themeNameChanged();
  void themeListChanged();
  void themePreviewChanged();
  void isPreviewingChanged();
  void previewThemeNameChanged();

private:
  explicit Colors(QObject *parent = nullptr);

  configs::ColorConfigMetadata *m_configMetadata;
  configs::ColorConfigColors *m_colors;

  configs::ColorConfigMetadata *m_previewConfigMetadata = nullptr;
  configs::ColorConfigColors *m_previewColors = nullptr;
  bool m_isPreviewing = false;

  QFileSystemWatcher *m_fileWatcher;
  QFileSystemWatcher *m_themeSelectorWatcher;

  QList<QString> m_themeDb;

  QString m_configPath = utils::Paths::instance()->config();
  QString m_themeName = "default";
  QString m_previewName = "default";

  void setIsPreviewing(bool value);

  QDir checkConfigPath();
  void writeConfigToPath(const QString &name);
  void buildThemeDb();
  void attachFileWatcher();
  void resetConfigs();
  void loadFromFile(const QString &themeName,
                    ColorObjPayload *payload = nullptr);

  void onFileWatcherChanged(const QString &path);
  void onThemeSelectorFileChanged(const QString &path);

  QString getActiveTheme();
  void setActiveTheme(const QString &themeName);
  void setPreviewThemeName(const QString &themeName);
};

} // namespace myqmlplugin
