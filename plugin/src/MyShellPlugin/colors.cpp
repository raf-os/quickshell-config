#include "colors.h"
#include "metaiterate.h"

#include <qdebug.h>
#include <qdir.h>
#include <qdiriterator.h>
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
#include <qobject.h>
#include <qprocess.h>
#include <qstringview.h>

namespace myqmlplugin {
Colors::Colors(QObject *parent) : QObject(parent) {
  m_configMetadata = new configs::ColorConfigMetadata(this);
  m_colors = new configs::ColorConfigColors(this);
  m_fileWatcher = new QFileSystemWatcher(this);

  QObject::connect(m_fileWatcher, &QFileSystemWatcher::fileChanged, this,
                   &Colors::onFileWatcherChanged);

  loadConfig();
  buildThemeDb();
}

configs::ColorConfigMetadata *Colors::metadata() const {
  return m_configMetadata;
}

configs::ColorConfigColors *Colors::colors() const { return m_colors; }

QString Colors::configPath() const { return m_configPath; }
void Colors::setConfigPath(const QString &path) {
  if (path == m_configPath)
    return;
  m_configPath = path;
  emit configPathChanged();

  buildThemeDb();
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

  attachFileWatcher();
  loadConfig();
}

QDir Colors::checkConfigPath() {
  auto dir = QDir(m_configPath + "/themes");
  if (!dir.exists()) {
    dir.mkpath(".");
  }
  return dir;
}

void Colors::buildThemeDb() {
  auto dir = checkConfigPath();

  m_themeDb.clear();
  m_themeDb.append("default");

  QDirIterator it(dir.canonicalPath(), {"*.json"}, QDir::Files);
  while (it.hasNext()) {
    QFile f(it.next());
    m_themeDb.append(f.fileName());
  }
}

void Colors::attachFileWatcher() {
  m_fileWatcher->removePaths(m_fileWatcher->files());

  auto dir = checkConfigPath();
  QFile tf(dir.canonicalPath() + "/" + m_themeName + ".json");
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

void Colors::onFileWatcherChanged(const QString &path) { loadConfig(); }

void Colors::loadConfig() {
  auto dir = checkConfigPath();
  QFile themeFile(dir.canonicalPath() + "/" + m_themeName + ".json");
  if (!themeFile.exists()) {
    resetConfigs();
    return;
  }

  if (!themeFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning() << "myqmlplugin::Colors::loadConfig: Error reading theme "
                  "file json.";
    return;
  }

  QByteArray tfContent;

  while (!themeFile.atEnd()) {
    QByteArray line = themeFile.readLine();
    tfContent.append(line);
  }

  QJsonParseError *error = nullptr;
  auto tfJson = QJsonDocument::fromJson(tfContent, error);

  themeFile.close();

  if (error != nullptr) {
    qWarning() << "myqmlplugin::Colors::loadConfig: Error parsing JSON: "
               << error->errorString();
    return;
  }

  if (tfJson.isObject()) {
    auto themeData = tfJson.object();
    auto confMetaObj = m_configMetadata->metaObject();
    for (auto i = confMetaObj->superClass()->propertyCount();
         i < confMetaObj->propertyCount(); ++i) {
      QMetaProperty prop = confMetaObj->property(i);
      if (auto it = themeData.constFind(prop.name());
          it != themeData.constEnd()) {
        prop.write(m_configMetadata, it->toString(""));
      } else {
        if (prop.isResettable()) {
          prop.reset(m_configMetadata);
        }
      }
    }
    auto colData = themeData["colors"].isObject()
                       ? themeData["colors"].toObject()
                       : QJsonObject();

    auto colMeta = m_colors->metaObject();
    for (auto i = colMeta->superClass()->propertyCount();
         i < colMeta->propertyCount(); ++i) {
      QMetaProperty prop = colMeta->property(i);
      if (auto it = colData.constFind(prop.name()); it != colData.constEnd()) {
        prop.write(m_colors, it->toString(""));
      } else {
        if (prop.isResettable()) {
          prop.reset(m_colors);
        }
      }
    }
  } else {
    qWarning()
        << "myqmlplugin::Colors::loadConfig: Invalid JSON data provided.";
    return;
  }
}

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
