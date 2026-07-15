#pragma once

#include <qtclasshelpermacros.h>
#include <qtypes.h>

#include "qwayland-hyprland-toplevel-mapping-v1.h"
#include "toplevelhandle.h"
#include "wayland-hyprland-toplevel-mapping-v1-client-protocol.h"

namespace ns::hyprland::toplevels {
class HyprlandToplevelMappingHandle
    : QtWayland::hyprland_toplevel_window_mapping_handle_v1 {
  Q_DISABLE_COPY_MOVE(HyprlandToplevelMappingHandle)

public:
  explicit HyprlandToplevelMappingHandle(
      ToplevelHandle                               *handle,
      ::hyprland_toplevel_window_mapping_handle_v1 *mapping)
      : QtWayland::hyprland_toplevel_window_mapping_handle_v1(mapping),
        m_handle(handle) {};
  ~HyprlandToplevelMappingHandle() override;

protected:
  void hyprland_toplevel_window_mapping_handle_v1_window_address(
      quint32 addressHi,
      quint32 addressLo) override;
  void hyprland_toplevel_window_mapping_handle_v1_failed() override;

private:
  ToplevelHandle *m_handle;
};
} // namespace ns::hyprland::toplevels
