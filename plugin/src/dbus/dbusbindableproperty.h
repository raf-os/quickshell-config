#pragma once

#include <qobject.h>

namespace ns::dbus {
template <typename T> struct MetaMember;

template <typename T, typename C> struct MetaMember<T C::*> {
  using Class = C;
  using Type  = T;
};

template <typename T> struct BindableParams;

// Extracts bindable meta information from a given bindable
template <template <typename, typename, auto, auto> class B, typename C,
    typename T, auto O, auto S>
struct BindableParams<B<C, T, O, S>> {
  using Class = C;
  using Type  = T;
};

template <typename Bindable> struct BindableType {
  using Meta = BindableParams<Bindable>;
  using Type = Meta::Type;
};
} // namespace ns::dbus
