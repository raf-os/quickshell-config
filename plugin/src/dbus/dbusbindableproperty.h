#pragma once

#include <cstddef>
#include <type_traits>

#include <bit>
#include <qobject.h>
#include <qtclasshelpermacros.h>
#include <qtdeprecationdefinitions.h>

#include "dbuspropertygroup.h"

namespace ns::dbus {
template <typename T> struct MetaMember;

template <typename T, typename C> struct MetaMember<T C::*> {
  using Class = C;
  using Type  = T;
};

template <typename T> struct BindableParams;

// Extracts bindable meta information from a given bindable
template <template <typename, typename, auto, auto> class B,
          typename C,
          typename T,
          auto O,
          auto S>
struct BindableParams<B<C, T, O, S>> {
  using Class = C;
  using Type  = T;
};

template <typename Bindable> struct BindableType {
  using Meta = BindableParams<Bindable>;
  using Type = Meta::Type;
};

template <typename T,
          auto Offset,
          auto BindablePtr,
          auto updateFnPtr,
          auto GroupPtr>
class DBusBindableProperty : public DBusPropertyBase {
  using PtrMeta      = MetaMember<decltype(BindablePtr)>;
  using Owner        = PtrMeta::Class;
  using Bindable     = PtrMeta::Type;
  using BindableType = BindableType<Bindable>::Type;
  using BaseType     = std::conditional_t<std::is_void_v<T>, BindableType, T>;

public:
  explicit DBusBindableProperty(const char *name) : m_name(name) {
    group()->attachProperty(this);
  }
  [[nodiscard]] QString name() const override { return m_name; }

  void store() {
    if constexpr (updateFnPtr != nullptr) {
      (owner()->*updateFnPtr)();
    }
  }

private:
  QString m_name;

  // QObjectBindableProperty does the same thing but with reinterpret_cast
  // instead of bit_cast
  // TODO: Tests
  [[nodiscard]] constexpr Owner *owner() const {
    auto *self = std::bit_cast<char *>(this);
    return std::bit_cast<Owner *>(self - Offset());
  }

  [[nodiscard]] constexpr Bindable *bindable() const {
    static_assert(std::is_member_object_pointer_v<decltype(BindablePtr)>);
    return &(owner()->*BindablePtr);
  }
  [[nodiscard]] constexpr DBusPropertyGroup *group() const {
    static_assert(std::is_member_object_pointer_v<decltype(GroupPtr)>);
    return &(owner()->*GroupPtr);
  }
};

#define DBUS_PROPERTY_BINDING_P(                                               \
    Class, Type, property, bindable, updated, group, name)                     \
  static constexpr size_t _ns_property_##property##_offset() {                 \
    QT_WARNING_PUSH QT_WARNING_DISABLE_INVALID_OFFSETOF return offsetof(       \
        Class, property);                                                      \
    QT_WARNING_POP                                                             \
  }                                                                            \
                                                                               \
  ns::dbus::DBusBindableProperty<Type,                                         \
                                 &Class::_ns_property_##property##_offset,     \
                                 &Class::bindable,                             \
                                 updated,                                      \
                                 &Class::group>                                \
      property{name};

#define DBUS_PROPERTY_BINDING(Class, Type, property, bindable, group, name)    \
  DBUS_PROPERTY_BINDING_P(                                                     \
      Class, Type, property, bindable, nullptr, group, name);
} // namespace ns::dbus
