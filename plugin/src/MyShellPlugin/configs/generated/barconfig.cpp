#include "barconfig.h"

#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace myqmlplugin{
namespace configs{
// BEGIN CLASS [[ Sizes ]]
Sizes::Sizes(QObject *parent): QObject(parent) {}

int Sizes::innerHeight() const { return m_innerHeight; }

void Sizes::setInnerHeight(int value) {
	m_innerHeight = value;
}
QBindable<int> Sizes::bindableInnerHeight() { return &m_innerHeight; }

int Sizes::trayMenuWidth() const { return m_trayMenuWidth; }

void Sizes::setTrayMenuWidth(int value) {
	m_trayMenuWidth = value;
}
QBindable<int> Sizes::bindableTrayMenuWidth() { return &m_trayMenuWidth; }

int Sizes::launcherWidth() const { return m_launcherWidth; }

void Sizes::setLauncherWidth(int value) {
	m_launcherWidth = value;
}
QBindable<int> Sizes::bindableLauncherWidth() { return &m_launcherWidth; }

int Sizes::mediaInfoWidth() const { return m_mediaInfoWidth; }

void Sizes::setMediaInfoWidth(int value) {
	m_mediaInfoWidth = value;
}
QBindable<int> Sizes::bindableMediaInfoWidth() { return &m_mediaInfoWidth; }
// END CLASS [[ Sizes ]]

// BEGIN CLASS [[ BarConfig ]]
BarConfig::BarConfig(QObject *parent): QObject(parent) {}

Sizes *BarConfig::sizes() const { return m_sizes; }
// END CLASS [[ BarConfig ]]
} // namespace configs
} // namespace myqmlplugin
