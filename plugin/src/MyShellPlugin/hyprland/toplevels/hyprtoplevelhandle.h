#pragma once

#include <functional>

#include <qtclasshelpermacros.h>
#include <qtypes.h>

#include "qwayland-hyprland-toplevel-mapping-v1.h"
#include "wayland-hyprland-toplevel-mapping-v1-client-protocol.h"

namespace ns::hyprland::toplevels {
class HyprlandToplevelMappingHandle
    : QtWayland::hyprland_toplevel_window_mapping_handle_v1 {
  using ManagerCallbackFn = std::function<void(void *, quint64 address)>;

public:
  explicit HyprlandToplevelMappingHandle(
      ManagerCallbackFn                             callbackFn,
      void                                         *handle,
      ::hyprland_toplevel_window_mapping_handle_v1 *mapping)
      : QtWayland::hyprland_toplevel_window_mapping_handle_v1(mapping),
        m_handle(handle),
        m_callbackFn(callbackFn) {};
  ~HyprlandToplevelMappingHandle() override;

  Q_DISABLE_COPY_MOVE(HyprlandToplevelMappingHandle)

protected:
  void hyprland_toplevel_window_mapping_handle_v1_window_address(
      quint32 addressHi,
      quint32 addressLo) override;
  void hyprland_toplevel_window_mapping_handle_v1_failed() override;

private:
  void             *m_handle;
  ManagerCallbackFn m_callbackFn = nullptr;
};
} // namespace ns::hyprland::toplevels
