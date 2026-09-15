#pragma once

#include <qdir.h>
#include <qjsengine.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlengine.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

#include "ns_utils_shared_global.h"

namespace ns::utils {
class NS_UTILS_EXPORT Paths : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  Q_PROPERTY(QString home READ home CONSTANT)
  Q_PROPERTY(
      QString config READ default NOTIFY configChanged BINDABLE bindableConfig)
  Q_PROPERTY(
      QString state READ default NOTIFY stateChanged BINDABLE bindableState)
  Q_PROPERTY(
      QString cache READ default NOTIFY cacheChanged BINDABLE bindableCache)
  Q_PROPERTY(QString data READ default NOTIFY dataChanged BINDABLE bindableData)
  Q_PROPERTY(QString hyprConfig READ default NOTIFY hyprConfigChanged BINDABLE
          bindableHyprConfig)

public:
  static Paths *instance();
  static Paths *create(QQmlEngine *qmlEngine, QJSEngine * /*unused*/);

  [[nodiscard]] QString            home() const;
  [[nodiscard]] QBindable<QString> bindableConfig() const;
  [[nodiscard]] QBindable<QString> bindableState() const;
  [[nodiscard]] QBindable<QString> bindableCache() const;
  [[nodiscard]] QBindable<QString> bindableData() const;
  [[nodiscard]] QBindable<QString> bindableHyprConfig() const;

signals:
  void configChanged();
  void stateChanged();
  void cacheChanged();
  void dataChanged();
  void hyprConfigChanged();

private:
  explicit Paths(QObject *parent = nullptr);

  const QString m_home        = QDir::homePath();
  const QString m_shellFolder = "/nightshell";

  void setupDefaultPaths();
  void ensurePathsExist();

  Q_OBJECT_BINDABLE_PROPERTY(Paths, QString, b_config, &Paths::configChanged)
  Q_OBJECT_BINDABLE_PROPERTY(Paths, QString, b_state, &Paths::stateChanged)
  Q_OBJECT_BINDABLE_PROPERTY(Paths, QString, b_cache, &Paths::cacheChanged)
  Q_OBJECT_BINDABLE_PROPERTY(Paths, QString, b_data, &Paths::dataChanged)
  Q_OBJECT_BINDABLE_PROPERTY(
      Paths, QString, b_hyprConfig, &Paths::hyprConfigChanged)
};
} // namespace ns::utils
