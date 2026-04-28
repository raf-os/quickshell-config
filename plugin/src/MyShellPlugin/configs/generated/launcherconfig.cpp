#include "launcherconfig.h"

#include <qlist.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qstring.h>
#include <qtmetamacros.h>

namespace myqmlplugin{
namespace configs{
// BEGIN CLASS [[ LauncherConfig ]]
LauncherConfig::LauncherConfig(QObject *parent): QObject(parent) {}

bool LauncherConfig::enabled() const { return m_enabled; }

void LauncherConfig::setEnabled(bool value) {
	m_enabled = value;
}
QBindable<bool> LauncherConfig::bindableEnabled() { return &m_enabled; }
void LauncherConfig::resetEnabled() { m_enabled = true; }

int LauncherConfig::width() const { return m_width; }

void LauncherConfig::setWidth(int value) {
	m_width = value;
}
QBindable<int> LauncherConfig::bindableWidth() { return &m_width; }
void LauncherConfig::resetWidth() { m_width = 420; }

QString LauncherConfig::commandPrefix() const { return m_commandPrefix; }

void LauncherConfig::setCommandPrefix(const QString &value) {
	m_commandPrefix = value;
}
QBindable<QString> LauncherConfig::bindableCommandPrefix() { return &m_commandPrefix; }
void LauncherConfig::resetCommandPrefix() { m_commandPrefix = "/"; }

QList<QString> &LauncherConfig::favoriteApps() { return m_favoriteApps; }void LauncherConfig::setFavoriteApps(const QList<QString> &newValue) {
	if (m_favoriteApps == newValue)
		return;
	m_favoriteApps = newValue;
	emit favoriteAppsChanged();
}
// END CLASS [[ LauncherConfig ]]
} // namespace configs
} // namespace myqmlplugin
