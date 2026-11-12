#pragma once

#include <vector>

#include <qbytearrayview.h>
#include <qcontainerfwd.h>
#include <qdbusabstractinterface.h>
#include <qhash.h>
#include <qobject.h>
#include <qpointer.h>
#include <qproperty.h>
#include <qtmetamacros.h>

#include "dbus_introspection_properties.h"
#include "templates.h"

namespace ns::dbus {
class DBusBindablePropertyBase;

struct DBusBindableMeta {
  QByteArrayView propertyName;
};

class DBusBindableGroupBase : public QObject {
  Q_OBJECT

public:
  explicit DBusBindableGroupBase(
      std::vector<DBusBindablePropertyBase> properties,
      QObject                              *parent = nullptr);
  explicit DBusBindableGroupBase(QObject *parent = nullptr);

  void setInterface(QDBusAbstractInterface *interface);

  [[nodiscard]] bool pending() const;

signals:
  void pendingChanged();

private slots:
  void onPropertiesChanged(const QString &interface,
      const QVariantMap                  &changedProperties,
      const QStringList                  &invalidatedProperties);
  void onInterfaceDestroyed();

private:
  bool m_pending = false;

  QPointer<QObject>            m_parent              = nullptr;
  QDBusAbstractInterface      *m_interface           = nullptr;
  DBusIntrospectionProperties *m_propertiesInterface = nullptr;

  QHash<QString, DBusBindablePropertyBase *> m_properties;
};

class DBusBindablePropertyBase : QObject {
  Q_OBJECT

public:
  explicit DBusBindablePropertyBase(DBusBindableGroupBase *group);

  virtual QLatin1StringView propertyName() = 0;
  virtual void              changed()      = 0;

  bool pending = false;

protected:
  DBusBindableGroupBase *m_group;
};

#define DBUS_AUTO_PROPERTY_DECL(Type, Name)                                    \
public:                                                                        \
  Q_SIGNAL void Name##Changed();                                               \
                                                                               \
private:                                                                       \
  DBusBindableMeta Name { .propertyName = Name; };

#define DBUS_AUTO_PROPERTY(Type, Name)                                         \
  Q_PROPERTY(                                                                  \
      Type Name READ default NOTIFY Name##Changed BINDABLE bindable_##Name)    \
public:                                                                        \
  [[nodiscard]] QBindable<Type> bindable_##Name() const;

#define DBUS_AUTO_PROPERTY_WRITABLE(Type, Name)                                \
  Q_PROPERTY(                                                                  \
      Type Name READ default WRITE default NOTIFY Name##Changed BINDABLE       \
                                                  bindable_##Name)             \
public:                                                                        \
  [[nodiscard]] QBindable<Type> bindable__ #Name();
} // namespace ns::dbus
