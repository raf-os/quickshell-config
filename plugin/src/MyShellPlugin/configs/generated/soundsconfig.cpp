#include "soundsconfig.h"

#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qstring.h>
#include <qtmetamacros.h>

namespace myqmlplugin{
namespace configs{
// BEGIN CLASS [[ SoundsConfig ]]
SoundsConfig::SoundsConfig(QObject *root, QObject *parent): myqmlplugin::configs::CSerializable(root, parent) {}

QString SoundsConfig::getClassName() const { return m_className; }

bool SoundsConfig::enabled() const { return m_enabled; }

void SoundsConfig::setEnabled(bool value) {
	m_enabled = value;
}
QBindable<bool> SoundsConfig::bindableEnabled() { return &m_enabled; }
void SoundsConfig::resetEnabled() { m_enabled = true; }

QString SoundsConfig::theme() const { return m_theme; }

void SoundsConfig::setTheme(const QString &value) {
	m_theme = value;
}
QBindable<QString> SoundsConfig::bindableTheme() { return &m_theme; }
void SoundsConfig::resetTheme() { m_theme = "freedesktop"; }
// END CLASS [[ SoundsConfig ]]
} // namespace configs
} // namespace myqmlplugin
