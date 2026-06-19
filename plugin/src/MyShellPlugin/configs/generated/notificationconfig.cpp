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

int NotificationConfig::sidebarWidth() const { return m_sidebarWidth; }

void NotificationConfig::setSidebarWidth(int value) {
	m_sidebarWidth = value;
}
QBindable<int> NotificationConfig::bindableSidebarWidth() { return &m_sidebarWidth; }
void NotificationConfig::resetSidebarWidth() { m_sidebarWidth = 320; }
// END CLASS [[ NotificationConfig ]]
} // namespace configs
} // namespace myqmlplugin
