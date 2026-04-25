#include "keymapconfig.h"

#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace myqmlplugin{
namespace configs{
// BEGIN CLASS [[ KeyMapConfig ]]
KeyMapConfig::KeyMapConfig(QObject *parent): QObject(parent) {}

bool KeyMapConfig::enabled() const { return m_enabled; }

void KeyMapConfig::setEnabled(bool value) {
	m_enabled = value;
}
QBindable<bool> KeyMapConfig::bindableEnabled() { return &m_enabled; }
// END CLASS [[ KeyMapConfig ]]
} // namespace configs
} // namespace myqmlplugin
