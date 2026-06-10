#pragma once

#include <qcontainerfwd.h>
#include <qhash.h>
#include <qjsengine.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qlist.h>
#include <qlogging.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlengine.h>
#include <qqmlintegration.h>
#include <qqmlparserstatus.h>
#include <qtimer.h>
#include <qtmetamacros.h>

#include "generated/gen_includes.h" // IWYU pragma: keep
#include "iconfigserializer.h"

namespace myqmlplugin {
namespace configs {
class Config : public QObject, public IConfigSerializer {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON
  Q_INTERFACES(myqmlplugin::configs::IConfigSerializer)

  Q_PROPERTY(bool isSaving READ isSaving NOTIFY isSavingChanged)

#define X(Type, Name)                                                          \
  Q_PROPERTY(myqmlplugin::configs::Type *Name READ Name CONSTANT)              \
public:                                                                        \
  [[nodiscard]] Type *Name() const;                                            \
                                                                               \
private:                                                                       \
  Type *m_##Name = nullptr;
#include "generated/gen_types.def"
#undef X

public:
  static Config *instance() {
    static Config *s_instance = new Config();
    return s_instance;
  }

  static Config *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine) {
    auto inst = instance();
    if (qmlEngine)
      qmlEngine->setObjectOwnership(inst, QJSEngine::CppOwnership);
    return inst;
  }

  [[nodiscard]] bool isSaving() const;

  Q_INVOKABLE void saveConfigs();
  Q_INVOKABLE void earlyLoad() {};
  void commitSave() override;

signals:
  void sucessfulSave();
  void isSavingChanged();

private:
  explicit Config(QObject *parent = nullptr);

  QQmlEngine *m_engine = nullptr;
  const QString m_fileSuffix = "/configs.json";
  QHash<QString, QObject *> m_propertyTable;
  QTimer *m_saveTimer = nullptr;
  const int m_saveTimerInterval = 500;
  bool m_isSaving = false;

  bool readFromFile();
  bool readFromFile(QString filePath);

  bool saveToFile();
  bool saveToFile(QString filePath);

  QJsonObject iterateQObject(QObject *obj);
  void serializeFromJson(QObject *obj, const QJsonObject &jObj);
};
} // namespace configs
} // namespace myqmlplugin
