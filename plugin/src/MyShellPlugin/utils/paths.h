#pragma once

#include <QtQmlIntegration/qqmlintegration.h>
#include <qdir.h>
#include <qjsengine.h>
#include <qobject.h>
#include <qqmlengine.h>
#include <qtenvironmentvariables.h>
#include <qtmetamacros.h>

namespace myqmlplugin {
namespace utils {
class Paths : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  Q_PROPERTY(QString home READ home CONSTANT)
  Q_PROPERTY(QString config READ config NOTIFY configChanged)
  Q_PROPERTY(QString state READ state NOTIFY stateChanged)
  Q_PROPERTY(QString cache READ cache NOTIFY cacheChanged)
  Q_PROPERTY(QString hyprConfig READ hyprConfig NOTIFY hyprConfigChanged)

public:
  static Paths *instance() {
    static Paths *s_instance = new Paths();
    return s_instance;
  }

  static Paths *create(QQmlEngine *, QJSEngine *) { return instance(); }

  [[nodiscard]] QString home() const;
  [[nodiscard]] QString config() const;
  [[nodiscard]] QString state() const;
  [[nodiscard]] QString cache() const;
  [[nodiscard]] QString hyprConfig() const;

signals:
  void configChanged();
  void stateChanged();
  void cacheChanged();
  void hyprConfigChanged();

private:
  explicit Paths(QObject *parent = nullptr);

  const QString m_home = QDir::homePath();
  QString m_config =
      qEnvironmentVariable("XDG_CONFIG_HOME", m_home + "/.config") + "/myshell";
  QString m_state =
      qEnvironmentVariable("XDG_STATE_HOME", m_home + "/.state") + "/myshell";
  QString m_cache =
      qEnvironmentVariable("XDG_CACHE_HOME", m_home + "/.cache") + "/myshell";
  QString m_hyprConfig = m_home + "/.config/hypr";
};
} // namespace utils
} // namespace myqmlplugin
