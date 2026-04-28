#include "borderconfig.h"

#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace myqmlplugin{
namespace configs{
// BEGIN CLASS [[ BorderConfig ]]
BorderConfig::BorderConfig(QObject *parent): QObject(parent) {}

int BorderConfig::thickness() const { return m_thickness; }

void BorderConfig::setThickness(int value) {
	m_thickness = value;
}
QBindable<int> BorderConfig::bindableThickness() { return &m_thickness; }
void BorderConfig::resetThickness() { m_thickness = 8; }

int BorderConfig::rounding() const { return m_rounding; }

void BorderConfig::setRounding(int value) {
	m_rounding = value;
}
QBindable<int> BorderConfig::bindableRounding() { return &m_rounding; }
void BorderConfig::resetRounding() { m_rounding = 16; }
// END CLASS [[ BorderConfig ]]
} // namespace configs
} // namespace myqmlplugin
