#include "paths.h"
#include <qdir.h>
#include <qhashfunctions.h>
#include <qlogging.h>
#include <qobject.h>
#include <qtenvironmentvariables.h>

namespace myqmlplugin {
namespace utils {
Paths::Paths(QObject *parent) : QObject(parent) {
  QDir configDir(m_config);
  if (!configDir.exists()) {
    configDir.mkpath(".");
  }
}

QString Paths::home() const { return m_home; }
QString Paths::config() const { return m_config; }
QString Paths::state() const { return m_state; }
QString Paths::cache() const { return m_cache; }
QString Paths::hyprConfig() const { return m_hyprConfig; }
} // namespace utils
} // namespace myqmlplugin
