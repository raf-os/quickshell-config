#include "colorservice.h"
#include <qdir.h>
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
ColorService::ColorService(QObject *parent) : QObject(parent) {
  m_configMetadata = new configs::ColorConfigMetadata(this);
  m_colors = new configs::ColorConfigColors(this);
}

configs::ColorConfigMetadata *ColorService::metadata() const {
  return m_configMetadata;
}

configs::ColorConfigColors *ColorService::colors() const { return m_colors; }

QString ColorService::configPath() const { return m_configPath; }
void ColorService::setConfigPath(const QString &path) {
  if (path == m_configPath)
    return;
  m_configPath = path;
  emit configPathChanged();
}

void ColorService::loadConfig() {
  auto dir = QDir(m_configPath + "/config");
  if (!dir.exists()) {
    dir.mkpath(".");
  }
  QFile themeFile(dir.canonicalPath() + "/theme.json");
  if (!themeFile.exists())
    return;

  if (!themeFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning() << "myqmlplugin::ColorService::loadConfig: Error reading theme "
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
    qWarning() << "myqmlplugin::ColorService::loadConfig: Error parsing JSON: "
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
        << "myqmlplugin::ColorService::loadConfig: Invalid JSON data provided.";
    return;
  }
}

void ColorService::saveConfig() {
  auto dir = QDir(m_configPath, "/config");
  if (!dir.exists()) {
    dir.mkpath(".");
  }

  const QString filePath = dir.canonicalPath() + "/theme.json";
  QLockFile lockFile(filePath + ".lock");
  if (lockFile.tryLock(1000)) {
    QFile themeFile(filePath);
    if (!themeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
      qWarning()
          << "myqmlplugin::ColorService::saveConfig: Error opening theme "
             "file to save.";
      return;
    }

    QJsonObject docRoot;

    auto confMeta = m_configMetadata->metaObject();
  }
}
} // namespace myqmlplugin
