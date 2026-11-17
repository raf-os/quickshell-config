#pragma once

#include <cstddef>
#include <utility>
#include <vector>

#include <bit>
#include <qbytearrayview.h>
#include <qcontainerfwd.h>
#include <qdbusabstractinterface.h>
#include <qdbusargument.h>
#include <qdbuspendingcall.h>
#include <qhash.h>
#include <qhashfunctions.h>
#include <qlist.h>
#include <qloggingcategory.h>
#include <qmap.h>
#include <qobject.h>
#include <qoverload.h>
#include <qpointer.h>
#include <qproperty.h>
#include <qstringview.h>
#include <qtclasshelpermacros.h>
#include <qtmetamacros.h>
#include <qvariant.h>

#include "dbus_introspection_properties.h"
#include "dbusbindableproperty.h"
#include "ns_dbus_debug.h"
#include "templates.h"

namespace ns::dbus {
class DBusBindableGroup;

struct DBusBindableMeta {
  bool writable  = false;
  bool hasSignal = true;
};

class DBusBindablePropertyBase {
public:
  DBusBindablePropertyBase()          = default;
  virtual ~DBusBindablePropertyBase() = default;
  Q_DISABLE_COPY_MOVE(DBusBindablePropertyBase)

  virtual QString     propertyNameStr() const                  = 0;
  virtual QStringView propertyName() const                     = 0;
  virtual QVariant    toVariant()                              = 0;
  virtual void        serialize(const QVariant &variant) const = 0;
  virtual bool        isWritable() const                       = 0;

  QPointer<QDBusPendingCallWatcher> readWatcher  = nullptr;
  QPointer<QDBusPendingCallWatcher> writeWatcher = nullptr;
};

template <typename Class, typename T, auto Offset, auto Signal = nullptr>
class DBusBindable : public DBusBindablePropertyBase {
  using this_type = DBusBindable<Class, T, Offset, Signal>;
  static bool constexpr has_signal =
      !std::is_same_v<decltype(Signal), std::nullptr_t>;

private:
  [[nodiscard]] Class *owner() {
    auto *self = std::bit_cast<char *>(this);
    return std::bit_cast<Class *>(self - Offset());
  }
};

template <typename Class, typename T, TString16 Name, auto bindablePtr,
    auto groupPtr, auto offsetPtr>
class DBusBindableProperty : public DBusBindablePropertyBase {
  using ptr_meta   = MetaMember<decltype(bindablePtr)>;
  using owner_t    = ptr_meta::Class;
  using Bindable   = ptr_meta::Type;
  using bindable_t = BindableType<Bindable>::Type;

public:
  explicit DBusBindableProperty(DBusBindableMeta meta = {}) : m_meta(meta) {
    group()->attachProperty(this);
  }
  ~DBusBindableProperty() override {}

  [[nodiscard]] QString     propertyNameStr() const override { return Name; }
  [[nodiscard]] QStringView propertyName() const override { return Name; }

  void get() { group()->read(this); }
  void write() { group()->write(this); }
  void set(const T &value) {
    if (!m_meta.writable) return;

    bindable()->setValue(value);
    group()->write(this);
  }

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

  [[nodiscard]] constexpr Class *owner() const {
    auto *self = std::bit_cast<char *>(this);
    return std::bit_cast<Class *>(self - offsetPtr());
  }

  [[nodiscard]] constexpr DBusBindableGroup *group() const {
    return &(owner()->*groupPtr);
  }
  [[nodiscard]] constexpr Bindable *bindable() const {
    return &(owner()->*bindablePtr);
  }
};

class DBusBindableGroup : public QObject {
  Q_OBJECT

public:
  explicit DBusBindableGroup(QObject *parent = nullptr);

  void setInterface(QDBusAbstractInterface *interface);

  [[nodiscard]] bool pending() const;

  void write(DBusBindablePropertyBase *prop);
  void read(DBusBindablePropertyBase *prop);
  void attachProperty(DBusBindablePropertyBase *prop);

  void getAll();

signals:
  void pendingChanged();
  void getAllDone();

private slots:
  void onPropertiesChanged(const QString &interface,
      const QVariantMap                  &changedProperties,
      const QStringList                  &invalidatedProperties);
  void onInterfaceDestroyed();
  void onGetAllFinished(QDBusPendingCallWatcher *watcher);

private:
  bool m_pending      = false;
  bool m_getAllQueued = false;

  QObject                     *m_parent              = nullptr;
  QDBusAbstractInterface      *m_interface           = nullptr;
  DBusIntrospectionProperties *m_propertiesInterface = nullptr;

  QHash<QString, DBusBindablePropertyBase *> m_properties;

  void updateProperties(const QVariantMap &properties);
};

#define DBUS_BINDABLE_ATTACHED(Class)                                          \
  ns::dbus::DBusBindableGroup _dbus_bindableGroup{this};

#define DBUS_BINDABLE_MEMBER _dbus_bindableGroup

#define DBUS_BINDABLE_PROPERTY_INTERNAL_P(Class, Type, Property, Name, Meta)   \
private:                                                                       \
  QT_OBJECT_BINDABLE_PROPERTY_3(Class, Type, bi_##Property)                    \
  static constexpr size_t _ns_dbus_property_##Property##_internal_offset() {   \
    return offsetof(Class, Name);                                              \
  }                                                                            \
  ns::dbus::DBusBindableProperty<Class, Type, u## #Name,                       \
      &Class::bi_##Property, &Class::_dbus_bindableGroup,                      \
      &Class::_ns_dbus_property_##Property##_internal_offset>                  \
      Name{Meta};

#define DBUS_BINDABLE_PROPERTY_INTERNAL_5(Class, Type, Property, Name, Meta)   \
  DBUS_BINDABLE_PROPERTY_INTERNAL_P(Class, Type, Property, Name, Meta)

#define DBUS_BINDABLE_PROPERTY_INTERNAL_4(Class, Type, Property, Name)         \
  DBUS_BINDABLE_PROPERTY_INTERNAL_5(Class, Type, Property, Name, {})

#define DBUS_BINDABLE_PROPERTY_INTERNAL(...)                                   \
  QT_WARNING_PUSH QT_WARNING_DISABLE_INVALID_OFFSETOF QT_OVERLOADED_MACRO(     \
      DBUS_BINDABLE_PROPERTY_INTERNAL, __VA_ARGS__) QT_WARNING_POP

// Property MUST have read/write accessors. Only thing actually preventing
// writes is the "const" modifier on the bindable getter.
#define DBUS_BINDABLE_PROPERTY_P(Class, Type, Property, Name, Meta)            \
  Q_PROPERTY(Type Property READ default WRITE default NOTIFY Property##Changed \
          BINDABLE                                           bindable##Name)   \
public:                                                                        \
  [[nodiscard]] QBindable<Type> bindable##Name() const {                       \
    return &b_##Property;                                                      \
  }                                                                            \
  Q_SIGNAL void Property##Changed();                                           \
                                                                               \
private:                                                                       \
  QT_OBJECT_BINDABLE_PROPERTY_4(                                               \
      Class, Type, b_##Property, &Class::Property##Changed)                    \
  static constexpr size_t _ns_dbus_property_##Property##_offset() {            \
    return offsetof(Class, Name);                                              \
  }                                                                            \
  ns::dbus::DBusBindableProperty<Class, Type, u## #Name, &Class::b_##Property, \
      &Class::_dbus_bindableGroup,                                             \
      &Class::_ns_dbus_property_##Property##_offset>                           \
      Name{Meta};

#define DBUS_BINDABLE_PROPERTY_5(Class, Type, Property, Name, Meta)            \
  DBUS_BINDABLE_PROPERTY_P(Class, Type, Property, Name, Meta)

#define DBUS_BINDABLE_PROPERTY_4(Class, Type, Property, Name)                  \
  DBUS_BINDABLE_PROPERTY_5(Class, Type, Property, Name, {})

#define DBUS_BINDABLE_PROPERTY(...)                                            \
  QT_WARNING_PUSH QT_WARNING_DISABLE_INVALID_OFFSETOF QT_OVERLOADED_MACRO(     \
      DBUS_BINDABLE_PROPERTY, __VA_ARGS__) QT_WARNING_POP

} // namespace ns::dbus
