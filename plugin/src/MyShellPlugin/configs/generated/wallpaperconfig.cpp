#include "wallpaperconfig.h"

#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace myqmlplugin{
namespace configs{
// BEGIN CLASS [[ WallpaperConfig ]]
WallpaperConfig::WallpaperConfig(QObject *root, QObject *parent): myqmlplugin::configs::CSerializable(root, parent) {}

QString WallpaperConfig::getClassName() const { return m_className; }

bool WallpaperConfig::enabled() const { return m_enabled; }

void WallpaperConfig::setEnabled(bool value) {
	m_enabled = value;
}
QBindable<bool> WallpaperConfig::bindableEnabled() { return &m_enabled; }
void WallpaperConfig::resetEnabled() { m_enabled = true; }
// END CLASS [[ WallpaperConfig ]]
} // namespace configs
} // namespace myqmlplugin
