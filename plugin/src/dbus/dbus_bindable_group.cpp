#include "dbus_bindable_group.h"

#include <qcontainerfwd.h>
#include <qdbusabstractinterface.h>
#include <qdbusextratypes.h>
#include <qdbuspendingcall.h>
#include <qdbuspendingreply.h>
#include <qhashfunctions.h>
#include <qlist.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qscopeguard.h>
#include <qstringview.h>
#include <qvariant.h>

#include "dbus_introspection_properties.h"
#include "ns_dbus_debug.h"

namespace ns::dbus {
DBusBindableGroup::DBusBindableGroup(
    const QList<QString> &properties, QObject *parent)
    : QObject(parent), m_parent(parent) {}

DBusBindableGroup::DBusBindableGroup(QObject *parent)
    : DBusBindableGroup({}, parent) {}

void DBusBindableGroup::write(DBusBindablePropertyBase *prop) {
  if (!prop || !m_interface) return;

  if (!prop->isWritable()) {
    qCWarning(NS_LOG_DBUS) << "Tried assigning value to readonly property:"
                           << prop->propertyName();
    return;
  }

  if (prop->writeWatcher) {
    delete prop->writeWatcher.get();
  }

  auto  pending      = m_propertiesInterface->Set(m_interface->interface(),
      prop->propertyNameStr(), QDBusVariant(prop->toVariant()));
  auto *call         = new QDBusPendingCallWatcher(pending, prop);
  prop->writeWatcher = call;

  auto cb = [prop](QDBusPendingCallWatcher *watcher) {
    QDBusPendingReply<> reply = *watcher;

    if (reply.isError()) {
      qCWarning(NS_LOG_DBUS)
          << "Error writing property" << prop->propertyName();
      qCWarning(NS_LOG_DBUS) << reply.error();
    }
    delete watcher;
  };

  QObject::connect(call, &QDBusPendingCallWatcher::finished, prop, cb);
}

void DBusBindableGroup::read(DBusBindablePropertyBase *prop) {
  if (!prop || !m_interface) return;

  if (prop->readWatcher) {
    delete prop->readWatcher.get();
  }

  auto pending = m_propertiesInterface->Get(
      m_interface->interface(), prop->propertyNameStr());
  auto *call        = new QDBusPendingCallWatcher(pending, prop);
  prop->readWatcher = call;

  auto cb = [prop](QDBusPendingCallWatcher *watcher) {
    QDBusPendingReply<QVariant> reply = *watcher;

    if (reply.isError()) {
      qCWarning(NS_LOG_DBUS)
          << "Error reading property" << prop->propertyName();
      qCWarning(NS_LOG_DBUS) << reply.error();
    } else {
      prop->serialize(reply.value());
    }

    delete watcher;
  };
}

void DBusBindableGroup::attachProperty(DBusBindablePropertyBase *prop) {
  m_properties.insert(prop->propertyNameStr(), prop);
}

void DBusBindableGroup::getAll() {
  if (!m_interface) {
    qCFatal(NS_LOG_DBUS) << "Called getAll() with out an attached interface.";
  }

  if (m_pending) {
    m_getAllQueued = true;
    return;
  }

  m_pending     = true;
  auto  pending = m_propertiesInterface->GetAll(m_interface->interface());
  auto *call    = new QDBusPendingCallWatcher(pending, this);

  QObject::connect(call, &QDBusPendingCallWatcher::finished, this,
      &DBusBindableGroup::onGetAllFinished);
}

void DBusBindableGroup::onGetAllFinished(QDBusPendingCallWatcher *watcher) {
  m_pending  = false;
  auto guard = qScopeGuard([this, watcher] {
    if (m_getAllQueued) {
      m_getAllQueued = false;
      getAll();
    }
    delete watcher;
  });

  const QDBusPendingReply<QVariantMap> reply = *watcher;

  if (reply.isError()) {
    qCWarning(NS_LOG_DBUS) << "Error updating properties of" << m_parent
                           << "via GetAll:" << reply.error();
    return;
  }

  updateProperties(reply.value());
}

void DBusBindableGroup::updateProperties(const QVariantMap &properties) {
  for (const auto &[key, value] : properties.asKeyValueRange()) {
    auto it = m_properties.find(key);
    if (it == m_properties.end()) continue;

    it.value()->serialize(value);
  }
}

void DBusBindableGroup::setInterface(QDBusAbstractInterface *interface) {
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
        &DBusBindableGroup::onInterfaceDestroyed);

    QObject::connect(m_propertiesInterface,
        &DBusIntrospectionProperties::PropertiesChanged, this,
        &DBusBindableGroup::onPropertiesChanged);

    getAll();
  }
}

void DBusBindableGroup::onPropertiesChanged(const QString &interface,
    const QVariantMap                                     &changedProperties,
    const QStringList &invalidatedProperties) {
  if (interface != m_interface->interface()) return;

  for (const auto &name : invalidatedProperties) {
    auto it = m_properties.find(name);
    if (it == m_properties.end()) continue;

    read(it.value());
  }

  updateProperties(changedProperties);
}

void DBusBindableGroup::onInterfaceDestroyed() {
  if (m_propertiesInterface) {
    delete m_propertiesInterface;
    m_propertiesInterface = nullptr;
  }
}
} // namespace ns::dbus
