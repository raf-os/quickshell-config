#include "paths.h"

#include <qdir.h>
#include <qjsengine.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlengine.h>
#include <qtenvironmentvariables.h>

namespace ns::utils {
Paths::Paths(QObject *parent) : QObject(parent) { setupDefaultPaths(); }

void Paths::setupDefaultPaths() {
  {
    QScopedPropertyUpdateGroup scopeGuard;

    b_config = qEnvironmentVariable("XDG_CONFIG_HOME", m_home + "/.config") +
               m_shellFolder;
    b_state  = qEnvironmentVariable("XDG_STATE_HOME", m_home + "/.config") +
               m_shellFolder;
    b_cache  = qEnvironmentVariable("XDG_CACHE_HOME", m_home + "/.config") +
               m_shellFolder;
    b_data   = DATADIR_DEFAULT_PATH;
    b_hyprConfig = m_home + "/.config/hypr";
  }

  ensurePathsExist();
}

void Paths::ensurePathsExist() {
  auto ensurePath = [this](const QString &path) {
    QDir dirCheck(path);
    if (!dirCheck.exists()) {
      dirCheck.mkpath(".");
    }
  };

  ensurePath(b_config.value());
  ensurePath(b_cache.value());
  ensurePath(b_state.value());
}

Paths *Paths::instance() {
  static Paths *s_instance = new Paths();
  return s_instance;
}

Paths *Paths::create(QQmlEngine *qmlEngine, QJSEngine * /*unused*/) {
  auto inst = instance();
  if (qmlEngine) qmlEngine->setObjectOwnership(inst, QJSEngine::CppOwnership);
  return inst;
}

QString            Paths::home() const { return m_home; }
QBindable<QString> Paths::bindableConfig() const { return &b_config; }
QBindable<QString> Paths::bindableCache() const { return &b_cache; }
QBindable<QString> Paths::bindableState() const { return &b_state; }
QBindable<QString> Paths::bindableData() const { return &b_data; }
QBindable<QString> Paths::bindableHyprConfig() const { return &b_hyprConfig; }
} // namespace ns::utils
