#pragma once

#include <qcontainerfwd.h>
#include <qfilesystemwatcher.h>
#include <qhash.h>
#include <qjsonobject.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qtimer.h>
#include <qtmetamacros.h>

#include "generated/gen_includes.h" // IWYU pragma: keep
#include "helpermacros.h"

namespace ns::configs {
Q_DECLARE_LOGGING_CATEGORY(logNSConfigs)

class Config : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  AUTO_MEYERS_SINGLETON_QML(Config)

#define X(Type, Name)                                                          \
  Q_PROPERTY(ns::configs::Type *Name READ Name CONSTANT)                       \
public:                                                                        \
  [[nodiscard]] Type *Name();                                                  \
                                                                               \
private:                                                                       \
  Type m_##Name{#Type, nullptr, this};
#include "generated/gen_types.def"
#undef X

public:
  Q_INVOKABLE void saveConfigs();

signals:
  void loaded();
  void saved();

private slots:
  void onConfigFileChanged(const QStringList & /*unused*/ = QStringList());

private:
  explicit Config(QObject *parent = nullptr);

  QHash<QString, QObject *> m_propertyTable;
  QFileSystemWatcher        m_fileWatcher;
  QTimer                    m_saveCooldown;
  bool                      m_isSaveQueued = false;

  bool readFromFile();
  bool readFromFile(const QString &path);

  bool saveToFile();
  bool saveToFile(const QString &path);

  void        serializeFromJson(QObject *target, const QJsonObject &payload);
  QJsonObject serializeToJson(QObject *target);

  void attachFileWatcher();
};
} // namespace ns::configs
