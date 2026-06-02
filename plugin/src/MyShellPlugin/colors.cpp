#include "colors.h"
#include "metaiterate.h"
#include "paths.h"

#include <qbuffer.h>
#include <qdebug.h>
#include <qdir.h>
#include <qdiriterator.h>
#include <qfileinfo.h>
#include <qfilesystemwatcher.h>
#include <qhashfunctions.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonparseerror.h>
#include <qlist.h>
#include <qlockfile.h>
#include <qlogging.h>
#include <qmetaobject.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qprocess.h>
#include <qstringview.h>

namespace myqmlplugin {
Colors::Colors(QObject *parent)
    : QObject(parent), m_configMetadata(new configs::ColorConfigMetadata(this)),
      m_colors(new configs::ColorConfigColors(this)),
      m_themeSelectorWatcher(new QFileSystemWatcher(this)) {
  m_fileWatcher = new QFileSystemWatcher(this);

  QObject::connect(m_fileWatcher, &QFileSystemWatcher::fileChanged, this,
                   &Colors::onFileWatcherChanged);
  QObject::connect(m_themeSelectorWatcher, &QFileSystemWatcher::fileChanged,
                   this, &Colors::onThemeSelectorFileChanged);

  m_themeName = getActiveTheme();
  loadFromFile(m_themeName);
  buildThemeDb();
  attachFileWatcher();
}

QString Colors::getActiveTheme() {
  QDir statePath = utils::Paths::instance()->state();
  QFile activeThemeFile(statePath.canonicalPath() + "/activetheme");

  if (!activeThemeFile.exists()) {
    setActiveTheme("default");
    return "default";
  }

  if (!activeThemeFile.open(QIODevice::ReadOnly)) {
    return "default";
  }

  QByteArray buffer(activeThemeFile.size(), Qt::Uninitialized);
  activeThemeFile.read(buffer.data(), buffer.size());

  activeThemeFile.close();

  return QString::fromUtf8(buffer);
}

void Colors::setActiveTheme(const QString &themeName) {
  QDir statePath = utils::Paths::instance()->state();
  QFile activeThemeFile(statePath.canonicalPath() + "/activetheme");

  if (!activeThemeFile.open(QIODevice::WriteOnly))
    return;

  QTextStream out(&activeThemeFile);
  out << themeName;
}

configs::ColorConfigMetadata *Colors::metadata() const {
  return m_configMetadata;
}

configs::ColorConfigColors *Colors::colors() const { return m_colors; }

configs::ColorConfigMetadata *Colors::metadataPreview() const {
  return m_previewConfigMetadata;
}
configs::ColorConfigColors *Colors::colorsPreview() const {
  return m_previewColors;
}

QString Colors::configPath() const { return m_configPath; }
void Colors::setConfigPath(const QString &path) {
  if (path == m_configPath)
    return;
  m_configPath = path;
  emit configPathChanged();

  m_themeName = getActiveTheme();

  buildThemeDb();
  loadFromFile(m_themeName);
}

QString Colors::themeName() const { return m_themeName; }
void Colors::setThemeName(const QString &name) {
  if (name == m_themeName)
    return;

  if (!m_themeDb.contains(name)) {
    qWarning() << "myqmlplugin::Colors::setThemeName: Theme " << name
               << " does not exist.";
    return;
  }
  m_themeName = name;
  emit themeNameChanged();

  loadFromFile(m_themeName);
  setActiveTheme(m_themeName);
  attachFileWatcher();

  emit colorsChanged();
}

QString Colors::previewThemeName() const { return m_previewName; }
void Colors::setPreviewThemeName(const QString &value) {
  if (value == m_previewName)
    return;

  m_previewName = value;
  emit previewThemeNameChanged();
}

QList<QString> Colors::themeList() const { return m_themeDb; }

QDir Colors::checkConfigPath() {
  auto dir = QDir(m_configPath + "/themes");
  if (!dir.exists()) {
    dir.mkpath(".");
  }
  return dir;
}

void Colors::buildThemeDb() {
  auto dir = checkConfigPath();
  QList<QString> newDb;

  // m_themeDb.clear();
  newDb.append("default");

  QDirIterator it(dir.canonicalPath(), {"*.json"}, QDir::Files);
  while (it.hasNext()) {
    QFileInfo f(it.next());
    if (!newDb.contains(f.baseName()))
      newDb.append(f.baseName());
  }

  if (newDb != m_themeDb)
    ;
  {
    m_themeDb = newDb;
    emit themeListChanged();
  }
}

void Colors::attachFileWatcher() {
  if (!m_fileWatcher->files().isEmpty())
    m_fileWatcher->removePaths(m_fileWatcher->files());

  auto dir = checkConfigPath();
  QFile tf(dir.canonicalPath() + "/" + m_themeName + ".json");

  QDir stateDir(utils::Paths::instance()->state());
  QFile activeThemeFile(stateDir.canonicalPath() + "/activetheme");

  if (activeThemeFile.exists()) {
    m_themeSelectorWatcher->addPath(stateDir.canonicalPath() + "/activetheme");
  }

  if (!tf.exists())
    return;

  auto succ =
      m_fileWatcher->addPath(dir.canonicalPath() + "/" + m_themeName + ".json");

  if (!succ) {
    qWarning() << "myqmlplugin::Colors::attachFileWatcher: Error attaching "
                  "file watcher to theme file.";
    return;
  }
}

void Colors::onFileWatcherChanged(const QString &path) {
  loadConfig();
  attachFileWatcher();
}

void Colors::onThemeSelectorFileChanged(const QString &path) {
  auto newName = getActiveTheme();

  if (m_themeName == newName)
    return;

  setThemeName(newName);
}

bool Colors::isPreviewing() const { return m_isPreviewing; }
void Colors::setIsPreviewing(bool value) {
  if (value == m_isPreviewing)
    return;

  m_isPreviewing = value;
  emit isPreviewingChanged();
}

void Colors::loadPreview(const QString &themeName, QObject *handler) {
  bool emitChange = false;
  if (!m_previewColors) {
    m_previewColors = new configs::ColorConfigColors(this);
    emitChange = true;
  }
  if (!m_previewConfigMetadata) {
    m_previewConfigMetadata = new configs::ColorConfigMetadata(this);
    emitChange = true;
  }

  // if (m_previewName == themeName)
  //   return;

  setIsPreviewing(true);
  setPreviewThemeName(themeName);
  emit themePreviewChanged();

  ColorObjPayload payload{.metadata = m_previewConfigMetadata,
                          .colors = m_previewColors};

  loadFromFile(themeName, &payload);

  if (handler) {
    QObject::connect(handler, &QObject::destroyed, this, &Colors::closePreview);
  }
}

void Colors::closePreview() {
  if (m_previewColors)
    m_previewColors->deleteLater();
  if (m_previewConfigMetadata)
    m_previewConfigMetadata->deleteLater();

  m_previewColors = nullptr;
  m_previewConfigMetadata = nullptr;

  setIsPreviewing(false);
  setPreviewThemeName(m_themeName);
  emit themePreviewChanged();
}

void Colors::loadFromFile(const QString &themeName, ColorObjPayload *payload) {
  configs::ColorConfigMetadata *meta =
      payload ? payload->metadata : m_configMetadata;
  configs::ColorConfigColors *colors = payload ? payload->colors : m_colors;

  if (!meta || !colors) {
    qWarning() << "myqmlplugin::Colors::loadFromFile: Missing payload objects.";
    return;
  }

  auto dir = checkConfigPath();
  QFile themeFile(dir.canonicalPath() + "/" + themeName + ".json");

  if (!themeFile.exists()) {
    utils::resetMetaObj(meta);
    utils::resetMetaObj(colors);
    return;
  }

  if (!themeFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning() << "myqmlplugin::Colors::loadFromFile: Error parsing theme json "
                  "for theme '"
               << themeName << "'.";
    return;
  }

  QByteArray buffer(themeFile.size(), Qt::Uninitialized);
  themeFile.read(buffer.data(), buffer.size());

  QJsonParseError *error = nullptr;
  auto tfJson = QJsonDocument::fromJson(buffer, error);

  themeFile.close();

  if (error != nullptr) {
    qWarning() << "myqmlplugin::Colors::loadConfig: Error parsing JSON: "
               << error->errorString();
    return;
  }

  if (tfJson.isObject()) {
    auto data = tfJson.object();

    auto confMetaObj = meta->metaObject();
    for (auto i = confMetaObj->superClass()->propertyCount();
         i < confMetaObj->propertyCount(); ++i) {
      QMetaProperty prop = confMetaObj->property(i);
      if (auto it = data.constFind(prop.name()); it != data.constEnd()) {
        prop.write(meta, it->toString(""));
      } else {
        if (prop.isResettable()) {
          prop.reset(meta);
        }
      }
    }

    auto columnData =
        data["colors"].isObject() ? data["colors"].toObject() : QJsonObject();
    auto colMetaObj = colors->metaObject();
    for (auto i = colMetaObj->superClass()->propertyCount();
         i < colMetaObj->propertyCount(); ++i) {
      QMetaProperty prop = colMetaObj->property(i);
      if (auto it = columnData.constFind(prop.name());
          it != columnData.constEnd()) {
        prop.write(colors, it->toString(""));
      } else {
        if (prop.isResettable()) {
          prop.reset(colors);
        }
      }
    }
  } else {
    qWarning() << "myqmlplugin::Colors::loadConfig: Invalid json was provided.";
    return;
  }
}

void Colors::loadConfig() { return loadFromFile(m_themeName); }

void Colors::commitSave() { saveConfig(); }

void Colors::saveConfig() { writeConfigToPath(m_themeName); }

void Colors::resetConfigs() {
  utils::resetMetaObj(m_configMetadata);
  utils::resetMetaObj(m_colors);
}

void Colors::writeConfigToPath(const QString &name) {
  auto dir = checkConfigPath();

  const QString filePath = dir.canonicalPath() + "/" + name + ".json";
  QLockFile lockFile(filePath + ".lock");
  if (lockFile.tryLock(1000)) {
    QFile themeFile(filePath);
    if (!themeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
      qWarning() << "myqmlplugin::Colors::saveConfig: Error opening theme "
                    "file to save.";
      return;
    }

    QJsonObject docRoot = utils::serializeMetaObjToJson(m_configMetadata);
    docRoot["colors"] = utils::serializeMetaObjToJson(m_colors);

    QTextStream out(&themeFile);
    out << QJsonDocument(docRoot).toJson(QJsonDocument::Compact);

    themeFile.close();
  } else {
    qWarning() << "myqmlplugin::Colors::saveConfig: Error acquiring lock for "
                  "theme file - settings will not be saved.";
  }
}
} // namespace myqmlplugin
