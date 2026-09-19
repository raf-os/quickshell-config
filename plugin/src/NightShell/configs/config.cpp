#include "config.h"

#include <QtCore>
#include <QtGui/qcolor.h>
#include <qbuffer.h>
#include <qcborcommon.h>
#include <qcontainerfwd.h>
#include <qdebug.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qfilesystemwatcher.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonparseerror.h>
#include <qjsonvalue.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qtimer.h>
#include <qvariant.h>

#include "paths.h"

#define CONFIG_FILE_NAME "/configs.json"

namespace ns::configs {
Q_LOGGING_CATEGORY(logNSConfigs, "ns.configs")

Config::Config(QObject *parent) : QObject(parent) {
#define X(Type, Name) m_propertyTable.insert(#Name, &m_##Name);
#include "generated/gen_types.def"
#undef X

  m_saveCooldown.setSingleShot(true);
  m_saveCooldown.setInterval(2000);

  QObject::connect(&m_saveCooldown, &QTimer::timeout, this, [this] {
    if (this->m_isSaveQueued) this->saveConfigs();
  });

  QObject::connect(&m_fileWatcher, &QFileSystemWatcher::fileChanged, this,
      &Config::onConfigFileChanged);

  onConfigFileChanged();
}

#define X(Type, Name)                                                          \
  Type *Config::Name() { return &m_##Name; }
#include "generated/gen_types.def"
#undef X

void Config::saveConfigs() {
  if (m_saveCooldown.isActive()) {
    m_isSaveQueued = true;
    return;
  }
}

bool Config::saveToFile() {
  auto path = QFileInfo(
      utils::Paths::instance()->bindableConfig().value() + CONFIG_FILE_NAME);
  return this->saveToFile(path.canonicalFilePath());
}

bool Config::saveToFile(const QString &path) {
  QJsonObject jbuff;
  for (auto it = m_propertyTable.cbegin(); it != m_propertyTable.cend(); ++it) {
    jbuff[it.key()] = serializeToJson(it.value());
  }

  QJsonDocument jDoc(jbuff);

  QSaveFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qCWarning(logNSConfigs) << "Error opening config file for saving.";
    return false;
  }

  QTextStream out(&file);
  out << jDoc.toJson(QJsonDocument::Indented);

  if (!file.commit()) {
    qCWarning(logNSConfigs) << "Failed commiting config save operation";
    return false;
  }

  emit saved();

  return true;
}

bool Config::readFromFile() {
  auto path = QFileInfo(
      utils::Paths::instance()->bindableConfig().value() + CONFIG_FILE_NAME);
  return this->readFromFile(path.canonicalFilePath());
}

bool Config::readFromFile(const QString &path) {
  QFile file(path);

  if (!file.exists()) {
    qCWarning(logNSConfigs) << "Config file" << path << "does not exist.";
    return false;
  }

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qCWarning(logNSConfigs) << "Unable to open file" << path << "for reading.";
    return false;
  }

  auto data = file.readAll();

  QJsonParseError error;
  auto            jDoc = QJsonDocument::fromJson(data, &error);

  if (error.error != QJsonParseError::NoError) {
    qCWarning(logNSConfigs)
        << "Failed parsing config json file:" << error.errorString();
    return false;
  }

  if (!jDoc.isObject()) {
    qCWarning(logNSConfigs)
        << "Error parsing json file - file is misconfigured.";
    return false;
  }

  auto jRootObject = jDoc.object();

  for (auto it = m_propertyTable.cbegin(); it != m_propertyTable.cend(); ++it) {
    const auto propName = it.key();
    auto       obj      = it.value();
    auto       jIt      = jRootObject.value(propName);
    if (jIt.isObject()) {
      serializeFromJson(obj, jIt.toObject());
    } else {
      serializeFromJson(obj, {});
    }
  }

  return true;
}

void Config::serializeFromJson(QObject *target, const QJsonObject &payload) {
  auto metaObj = target->metaObject();

  for (int i = metaObj->propertyOffset(); i < metaObj->propertyCount(); ++i) {
    auto metaProp = metaObj->property(i);
    if (!metaProp.isReadable() || !metaProp.isReadable()) continue;

    QMetaType  metaType = metaProp.metaType();
    auto       propName = metaProp.name();
    QJsonValue val      = payload.value(propName);

    if (metaType.flags().testFlag(QMetaType::PointerToQObject)) {
      QObject *child = qvariant_cast<QObject *>(metaProp.read(target));
      if (child) {
        serializeFromJson(
            child, val.isObject() ? val.toObject() : QJsonObject());
        continue;
      }
    }

    if (val.isUndefined()) {
      if (metaProp.isResettable()) metaProp.reset(target);
      continue;
    }

    if (val.isArray()) {
      QVariantList list;
      for (const QJsonValue &item : val.toArray()) {
        if (item.isString() &&
            metaProp.userType() == qMetaTypeId<QList<QColor>>())
        {
          list.append(QColor(item.toString()));
        } else {
          list.append(item.toVariant());
        }
      }
      metaProp.write(target, list);
    } else {
      if (metaProp.userType() == qMetaTypeId<QColor>())
        metaProp.write(target, QColor(val.toString()));
      else metaProp.write(target, val.toVariant());
    }
  }
}

QJsonObject Config::serializeToJson(QObject *target) {
  QJsonObject jbuff;
  auto        metaObj = target->metaObject();

  for (auto i = metaObj->propertyOffset(); i < metaObj->propertyCount(); ++i) {
    auto metaProp = metaObj->property(i);

    if (!metaProp.isReadable()) continue;

    QVariant value = metaProp.read(target);
    if (!value.isValid()) continue;

    QMetaType  metaType = metaProp.metaType();
    const auto propName = QString::fromLocal8Bit(metaProp.name());

    if (metaType.flags().testFlag(QMetaType::PointerToQObject)) {
      auto child = qvariant_cast<QObject *>(value);
      if (child) {
        jbuff[propName] = serializeToJson(child);
      }
    } else if (!value.canConvert<QString>() && value.canConvert<QVariantList>())
    {
      QJsonArray array;
      for (const QVariant &item : value.toList()) {
        array.append(QJsonValue::fromVariant(item));
      }
      jbuff[propName] = array;
    } else {
      jbuff[propName] = QJsonValue::fromVariant(value);
    }
  }

  return jbuff;
}

void Config::attachFileWatcher() {
  auto path =
      utils::Paths::instance()->bindableConfig().value() + CONFIG_FILE_NAME;

  if (!m_fileWatcher.files().contains(path)) m_fileWatcher.addPath(path);
}

void Config::onConfigFileChanged(const QString & /*unused*/) {
  if (readFromFile()) emit loaded();

  attachFileWatcher();
}
} // namespace ns::configs
