#include "dbuspropertygroup.h"

#include <qcontainerfwd.h>
#include <qdbusabstractinterface.h>
#include <qdbuserror.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qvariant.h>

#include "dbusutils.h"

namespace ns::dbus {
Q_LOGGING_CATEGORY(logDBusPropertyGroup, "ns.dbus.DBusPropertyGroup")

DBusPropertyGroup::DBusPropertyGroup(QObject *parent) : QObject(parent) {}

void DBusPropertyGroup::attachProperty(DBusPropertyBase *property) {
  m_properties.append(property);
}

void DBusPropertyGroup::setInterface(QDBusAbstractInterface *interface) {
  if (m_interface != nullptr) {
    auto conn = interface->connection();
    conn.disconnect(interface->service(),
                    interface->path(),
                    "org.freedesktop.DBus.Properties",
                    "PropertiesChanged",
                    this,
                    SLOT(onPropertiesChanged()));
  }

  if (interface != nullptr) {
    m_interface = interface;

    auto conn = interface->connection();
    conn.connect(interface->service(),
                 interface->path(),
                 "org.freedesktop.DBus.Properties",
                 "PropertiesChanged",
                 this,
                 SLOT(onPropertiesChanged()));
  }
}

void DBusPropertyGroup::onPropertiesChanged(
    const QString     &interfaceName,
    const QVariantMap &changedProperties,
    const QStringList &invalidatedProperties) {
  qCDebug(logDBusPropertyGroup)
      << "Interface:" << interfaceName
      << "\nChanged properties:" << changedProperties
      << "\nInvalidated properties:" << invalidatedProperties;
}

void DBusPropertyGroup::getAllAndUpdate() {
  if (m_interface == nullptr) {
    qCFatal(logDBusPropertyGroup)
        << "Attempted to update DBus properties from a disconnected group.";
  }

  asyncGetAll(*m_interface, [this](QVariantMap response, QDBusError error) {
    if (error.isValid()) {
      qCWarning(logDBusPropertyGroup)
          << "Error updating properties via GetAll." << error.message();
    } else {
      qCDebug(logDBusPropertyGroup) << "GetAll response:" << response;
    }
  });
}
} // namespace ns::dbus
