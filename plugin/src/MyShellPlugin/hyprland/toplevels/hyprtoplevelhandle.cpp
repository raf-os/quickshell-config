#include "hyprtoplevelhandle.h"

#include <qtypes.h>

#include "hyprtoplevelmanager.h"

namespace ns::hyprland::toplevels {
HyprlandToplevelMappingHandle::~HyprlandToplevelMappingHandle() {
  if (this->isInitialized()) this->destroy();
}

void HyprlandToplevelMappingHandle::
    hyprland_toplevel_window_mapping_handle_v1_window_address(
        quint32 addressHi,
        quint32 addressLo) {
  auto address = static_cast<quint64>(addressHi) << 32 | addressLo;
  HyprlandToplevelMappingManager::instance()->assignAddress(m_handle, address);
  delete this;
}

void HyprlandToplevelMappingHandle::
    hyprland_toplevel_window_mapping_handle_v1_failed() {
  delete this;
};
} // namespace ns::hyprland::toplevels
