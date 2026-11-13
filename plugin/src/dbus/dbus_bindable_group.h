#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>

#include <bit>
#include <qbytearrayview.h>
#include <qcontainerfwd.h>
#include <qdbusabstractinterface.h>
#include <qdbusargument.h>
#include <qdbuspendingcall.h>
#include <qhash.h>
#include <qhashfunctions.h>
#include <qlist.h>
#include <qobject.h>
#include <qoverload.h>
#include <qpointer.h>
#include <qproperty.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include <qvariant.h>

#include "dbus_introspection_properties.h"
#include "dbusbindableproperty.h"
#include "templates.h"

namespace ns::dbus {
class DBusBindablePropertyBase;

struct DBusBindableMeta {
  bool writable = false;
};

class DBusBindableGroup : public QObject {
  Q_OBJECT

public:
  explicit DBusBindableGroup(
      const QList<QString> &properties, QObject *parent = nullptr);
  explicit DBusBindableGroup(QObject *parent = nullptr);

  void setInterface(QDBusAbstractInterface *interface);

  [[nodiscard]] bool pending() const;

  void write(DBusBindablePropertyBase *prop);
  void read(DBusBindablePropertyBase *prop);
  void attachProperty(DBusBindablePropertyBase *prop);

signals:
  void pendingChanged();

private slots:
  void onPropertiesChanged(const QString &interface,
      const QVariantMap                  &changedProperties,
      const QStringList                  &invalidatedProperties);
  void onInterfaceDestroyed();
  void onGetAllFinished(QDBusPendingCallWatcher *watcher);

private:
  bool m_pending      = false;
  bool m_getAllQueued = false;

  QPointer<QObject>            m_parent              = nullptr;
  QDBusAbstractInterface      *m_interface           = nullptr;
  DBusIntrospectionProperties *m_propertiesInterface = nullptr;

  QHash<QString, DBusBindablePropertyBase *> m_properties;

  void getAll();
  void updateProperties(const QVariantMap &properties);
};

class DBusBindablePropertyBase : public QObject {
  Q_OBJECT

public:
  DBusBindablePropertyBase()                   = default;
  virtual ~DBusBindablePropertyBase() override = default;

  virtual QString     propertyNameStr() const                  = 0;
  virtual QStringView propertyName() const                     = 0;
  virtual QVariant    toVariant()                              = 0;
  virtual void        serialize(const QVariant &variant) const = 0;
  virtual bool        isWritable() const                       = 0;

  QPointer<QDBusPendingCallWatcher> readWatcher  = nullptr;
  QPointer<QDBusPendingCallWatcher> writeWatcher = nullptr;
};

template <typename T, TString16 Name, auto bindablePtr, auto groupPtr,
    auto offsetPtr>
class DBusBindableProperty : public DBusBindablePropertyBase {
  using ptr_meta      = MetaMember<decltype(bindablePtr)>;
  using owner_t       = ptr_meta::Class;
  using _bindable     = ptr_meta::Type;
  using bindable_t    = BindableType<_bindable>;
  using bindable_type = bindable_t::Type;

public:
  explicit DBusBindableProperty(DBusBindableMeta meta = {}) : m_meta(meta) {
    group()->attachProperty(this);
  }
  ~DBusBindableProperty() override {}

  [[nodiscard]] QString     propertyNameStr() const override { return Name; }
  [[nodiscard]] QStringView propertyName() const override { return Name; }

  void write() { group()->write(this); }

  [[nodiscard]] bool isWritable() const override { return m_meta.writable; }

  void serialize(const QVariant &variant) const override {
    T value = qdbus_cast<T>(variant);
    bindable()->setValue(std::move(value));
  }

  QVariant toVariant() override {
    return QVariant::fromValue(bindable()->value());
  }

private:
  DBusBindableMeta m_meta;

  [[nodiscard]] constexpr owner_t *owner() const {
    auto *self = std::bit_cast<char *>(this);
    return std::bit_cast<owner_t *>(self - offsetPtr());
  }

  [[nodiscard]] constexpr DBusBindableGroup *group() const {
    return &(owner()->*groupPtr);
  }
  [[nodiscard]] constexpr _bindable *bindable() const {
    return &(owner()->*bindablePtr);
  }
};

#define DBUS_BINDABLE_ATTACHED                                                 \
  ns::dbus::DBusBindableGroup _dbus_bindableGroup{this};

#define DBUS_BINDABLE_MEMBER _dbus_bindableGroup

// Property MUST have read/write accessors. Only thing actually preventing
// writes is the "const" modifier on the bindable getter.
#define DBUS_BINDABLE_PROPERTY_P(Class, Type, Property, Name, Meta)            \
  Q_PROPERTY(Type Property READ default WRITE default NOTIFY Property##Changed \
          BINDABLE                                           bindable##Name)   \
public:                                                                        \
  Q_SIGNAL void Property##Changed();                                           \
                                                                               \
private:                                                                       \
  QT_OBJECT_BINDABLE_PROPERTY_4(                                               \
      Class, Type, b_##Property, &Class::Property##Changed);                   \
  static constexpr size_t _ns_dbus_property_##Property##_offset() {            \
    return offsetof(Class, b_##Property);                                      \
  }                                                                            \
  ns::dbus::DBusBindableProperty<Type, u## #Name, &Class::b_##Property,        \
      &Class::_dbus_bindableGroup,                                             \
      &Class::_ns_dbus_property_##Property##_offset>                           \
      Name{Meta};                                                              \
                                                                               \
public:                                                                        \
  [[nodiscard]] QBindable<Type> bindable##Name() const { return &b_##Property; }

#define DBUS_BINDABLE_PROPERTY_5(Class, Type, Property, Name, Meta)            \
  DBUS_BINDABLE_PROPERTY_P(Class, Type, Property, Name, Meta)

#define DBUS_BINDABLE_PROPERTY_4(Class, Type, Property, Name)                  \
  DBUS_BINDABLE_PROPERTY_5(Class, Type, Property, Name, {})

#define DBUS_BINDABLE_PROPERTY(...)                                            \
  QT_WARNING_PUSH QT_WARNING_DISABLE_INVALID_OFFSETOF QT_OVERLOADED_MACRO(     \
      DBUS_BINDABLE_PROPERTY, __VA_ARGS__) QT_WARNING_POP

} // namespace ns::dbus
