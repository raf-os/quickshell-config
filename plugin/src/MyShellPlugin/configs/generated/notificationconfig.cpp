#include "notificationconfig.h"

#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace myqmlplugin{
namespace configs{
// BEGIN CLASS [[ NotificationConfig ]]
NotificationConfig::NotificationConfig(QObject *root, QObject *parent): myqmlplugin::configs::CSerializable(root, parent) {}

QString NotificationConfig::getClassName() const { return m_className; }

bool NotificationConfig::enabled() const { return m_enabled; }

void NotificationConfig::setEnabled(bool value) {
	m_enabled = value;
}
QBindable<bool> NotificationConfig::bindableEnabled() { return &m_enabled; }
void NotificationConfig::resetEnabled() { m_enabled = true; }
// END CLASS [[ NotificationConfig ]]
} // namespace configs
} // namespace myqmlplugin
