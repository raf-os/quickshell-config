#include "config.h"
#include "paths.h"

#include <qcolor.h>
#include <qcontainerfwd.h>
#include <qdebug.h>
#include <qdir.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonparseerror.h>
#include <qjsonvalue.h>
#include <qlist.h>
#include <qlogging.h>
#include <qmetaobject.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qstringview.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <qvariant.h>

namespace myqmlplugin {
namespace configs {
Config::Config(QObject *parent) : QObject(parent) {
  m_saveTimer = new QTimer(this);
  m_saveTimer->setSingleShot(true);
  m_saveTimer->setInterval(m_saveTimerInterval);
  QObject::connect(m_saveTimer, &QTimer::timeout, this, [this]() {
    this->saveToFile();
    m_isSaving = false;
    emit isSavingChanged();
  });

#define X(Type, Name)                                                          \
  m_##Name = new Type(this);                                                   \
  m_propertyTable.insert("##Name", m_##Name);
#include "generated/gen_types.def"
#undef X

  readFromFile();
}

#define X(Type, Name)                                                          \
  Type *Config::Name() const { return m_##Name; }
#include "generated/gen_types.def"
#undef X

QJsonObject Config::iterateQObject(QObject *obj) {
  QJsonObject jBuff;
  const QMetaObject *metaObj = obj->metaObject();

  for (int i = 0; i < metaObj->propertyCount(); ++i) {
    if (i < metaObj->propertyOffset())
      continue;

    QMetaProperty mProp = metaObject()->property(i);
    if (!mProp.isReadable())
      continue;

    QVariant value = mProp.read(obj);
    if (!value.isValid())
      continue;

    QMetaType metaType = mProp.metaType();
    const auto pname = QString::fromUtf8(mProp.name());

    if (metaType.flags().testFlag(QMetaType::PointerToQObject)) {
      QObject *childObj = qvariant_cast<QObject *>(value);
      if (childObj != nullptr) {
        jBuff[pname] = iterateQObject(childObj);
      }
    } else if (value.canConvert<QVariantList>()) {
      QJsonArray array;
      for (const QVariant &item : value.toList()) {
        array.append(QJsonValue::fromVariant(item));
      }
      jBuff[pname] = array;
    } else {
      jBuff[pname] = QJsonValue::fromVariant(value);
    }
  }

  return jBuff;
}

void Config::serializeFromJson(QObject *obj, const QJsonObject &jObj) {
  const QMetaObject *metaObj = obj->metaObject();

  for (int i = metaObj->propertyOffset(); i < metaObj->propertyCount(); ++i) {
    QMetaProperty mProp = metaObj->property(i);
    if (!mProp.isReadable() || !mProp.isWritable())
      continue;

    QMetaType mType = mProp.metaType();
    const QString pName = mProp.name();
    QJsonValue val = jObj.value(pName);

    if (mType.flags().testFlag(QMetaType::PointerToQObject)) {
      QObject *child = qvariant_cast<QObject *>(mProp.read(obj));
      if (child) {
        serializeFromJson(child,
                          val.isObject() ? val.toObject() : QJsonObject{});
      }
    } else {
      if (val.isUndefined()) {
        if (mProp.isResettable())
          mProp.reset(obj);
      } else if (val.isArray()) {
        QVariantList list;
        for (const QJsonValue &item : val.toArray()) {
          if (item.isString() &&
              mProp.userType() == qMetaTypeId<QList<QColor>>())
            list.append(QColor(item.toString()));
          else
            list.append(item.toVariant());
        }
        mProp.write(obj, list);
      } else {
        if (mProp.userType() == qMetaTypeId<QColor>())
          mProp.write(obj, QColor(val.toString()));
        else
          mProp.write(obj, val.toVariant());
      }
    }
  }
}

void Config::saveConfigs() {
  m_saveTimer->start();
  m_isSaving = true;
  emit isSavingChanged();
}

bool Config::readFromFile() {
  auto fpath = utils::Paths::instance()->config() + m_fileSuffix;
  return readFromFile(fpath);
}

bool Config::readFromFile(QString filePath) {
  QFile file(filePath);

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning()
        << "myqmlplugin::configs:readFromFile: Failed reading config json.";
    return false;
  }

  QByteArray loadData;
  while (!file.atEnd()) {
    auto line = file.readLine();
    loadData.append(line);
  }

  QJsonParseError *error = nullptr;
  auto jDoc = QJsonDocument::fromJson(loadData, error);

  if (error != nullptr) {
    qWarning() << "myqmlplugin::configs::readFromFile: Error parsing json:\n"
               << error->errorString();
    return false;
  }

  if (!jDoc.isObject()) {
    qWarning()
        << "myqmlplugin::configs::readFromFile: Json config file is malformed.";
    return false;
  }

  QJsonObject jsonRootObj = jDoc.object();

  for (auto it = m_propertyTable.cbegin(); it != m_propertyTable.cend(); ++it) {
    const auto kname = it.key();

    auto configObj = it.value();
    auto jObjIt = jsonRootObj.value(kname);
    if (jObjIt.isObject()) {
      serializeFromJson(configObj, jObjIt.toObject());
    } else {
      serializeFromJson(configObj, QJsonObject{});
    }
  }

  return true;
}

bool Config::saveToFile() {
  auto fpath = utils::Paths::instance()->config() + m_fileSuffix;
  return saveToFile(fpath);
}

bool Config::saveToFile(QString filePath) {
  QJsonObject jBuff;
  for (auto it = m_propertyTable.cbegin(); it != m_propertyTable.cend(); ++it) {
    const auto name = it.key();
    QJsonObject j = iterateQObject(it.value());
    jBuff[name] = j;
  }

  QJsonDocument jDoc(jBuff);

  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qWarning() << "myqmlplugin::configs::saveToFile: Failed saving configs to "
                  "json file.";
    return false;
  }

  QTextStream out(&file);
  out << jDoc.toJson(QJsonDocument::Indented);

  emit sucessfulSave();
  return true;
}
} // namespace configs
} // namespace myqmlplugin
