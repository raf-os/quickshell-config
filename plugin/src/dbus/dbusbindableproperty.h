#pragma once

#include <type_traits>

#include <qobject.h>
#include <qtclasshelpermacros.h>

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

template <typename T>
concept MemberFunction = std::is_member_function_pointer<T>::value;

template <typename T, auto bindablePtr, MemberFunction updateFnPtr>
class DBusBindableProperty : public DBusPropertyBase {
  using PtrMeta      = MetaMember<decltype(bindablePtr)>;
  using Owner        = PtrMeta::Class;
  using Bindable     = PtrMeta::Type;
  using BindableType = BindableType<Bindable>::Type;
  using BaseType     = std::conditional_t<std::is_void_v<T>, BindableType, T>;

public:
  explicit DBusBindableProperty(DBusPropertyGroup *group) {
    group->attachProperty(this);
  }
  [[nodiscard]] QString name() const override { return m_name; }

private:
  QString m_name;
};
} // namespace ns::dbus
