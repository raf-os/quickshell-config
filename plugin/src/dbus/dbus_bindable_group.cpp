#include "dbus_bindable_group.h"

#include <vector>

#include <qdbusabstractinterface.h>
#include <qobject.h>

#include "dbus_introspection_properties.h"

namespace ns::dbus {
DBusBindablePropertyBase::DBusBindablePropertyBase(DBusBindableGroupBase *group)
    : QObject(group), m_group(group) {}

DBusBindableGroupBase::DBusBindableGroupBase(
    std::vector<DBusBindablePropertyBase> properties, QObject *parent)
    : QObject(parent), m_parent(parent) {}

DBusBindableGroupBase::DBusBindableGroupBase(QObject *parent)
    : DBusBindableGroupBase({}, parent) {}

void DBusBindableGroupBase::setInterface(QDBusAbstractInterface *interface) {
  if (m_interface) {
    delete m_propertiesInterface;
    m_propertiesInterface = nullptr;
    QObject::disconnect(m_interface, nullptr, this, nullptr);
  }

  if (interface) {
    m_interface = interface;

    m_propertiesInterface = new DBusIntrospectionProperties(
        interface->service(), interface->path(), interface->connection(), this);

    QObject::connect(m_interface, &QObject::destroyed, this,
        &DBusBindableGroupBase::onInterfaceDestroyed);

    QObject::connect(m_propertiesInterface,
        &DBusIntrospectionProperties::PropertiesChanged, this,
        &DBusBindableGroupBase::onPropertiesChanged);
  }
}

void DBusBindableGroupBase::onPropertiesChanged(const QString &interface,
    const QVariantMap &changedProperties,
    const QStringList &invalidatedProperties) {}

void DBusBindableGroupBase::onInterfaceDestroyed() {
  if (m_propertiesInterface) {
    delete m_propertiesInterface;
    m_propertiesInterface = nullptr;
  }
}
} // namespace ns::dbus
