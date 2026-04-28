#pragma once

#include <qhash.h>
#include <qjsengine.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qlist.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlengine.h>
#include <qqmlintegration.h>
#include <qtimer.h>
#include <qtmetamacros.h>

#include "generated/gen_includes.h" // IWYU pragma: keep

namespace myqmlplugin {
namespace configs {
class Config : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  Q_PROPERTY(bool isSaving READ isSaving NOTIFY isSavingChanged)

#define X(Type, Name)                                                          \
  Q_PROPERTY(Type *Name READ Name)                                             \
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

  static Config *create(QQmlEngine *, QJSEngine *) { return instance(); }

  [[nodiscard]] bool isSaving() const;

  Q_INVOKABLE void saveConfigs();

signals:
  void sucessfulSave();
  void isSavingChanged();

private:
  explicit Config(QObject *parent = nullptr);

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
