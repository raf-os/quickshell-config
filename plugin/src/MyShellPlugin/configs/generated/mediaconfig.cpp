#include "mediaconfig.h"

#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qstring.h>
#include <qtmetamacros.h>

namespace myqmlplugin{
namespace configs{
// BEGIN CLASS [[ MediaConfig ]]
MediaConfig::MediaConfig(QObject *parent): QObject(parent) {}

bool MediaConfig::enabled() const { return m_enabled; }

void MediaConfig::setEnabled(bool value) {
	m_enabled = value;
}
QBindable<bool> MediaConfig::bindableEnabled() { return &m_enabled; }
void MediaConfig::resetEnabled() { m_enabled = true; }

bool MediaConfig::mediaPopoutEnabled() const { return m_mediaPopoutEnabled; }

void MediaConfig::setMediaPopoutEnabled(bool value) {
	m_mediaPopoutEnabled = value;
}
QBindable<bool> MediaConfig::bindableMediaPopoutEnabled() { return &m_mediaPopoutEnabled; }
void MediaConfig::resetMediaPopoutEnabled() { m_mediaPopoutEnabled = true; }

QString MediaConfig::defaultPlayer() const { return m_defaultPlayer; }

void MediaConfig::setDefaultPlayer(const QString &value) {
	m_defaultPlayer = value;
}
QBindable<QString> MediaConfig::bindableDefaultPlayer() { return &m_defaultPlayer; }
void MediaConfig::resetDefaultPlayer() { m_defaultPlayer = "Spotify"; }
// END CLASS [[ MediaConfig ]]
} // namespace configs
} // namespace myqmlplugin
