#include "layer_shell_integration.h"

#include <memory>

#include "layer_surface.h"
#include "wayland-wlr-layer-shell-unstable-v1-client-protocol.h"
#include "xdg_activation.h"

namespace ns::layershell {
WaylandLayerShellIntegration::WaylandLayerShellIntegration()
    : QtWaylandClient::QWaylandShellIntegrationTemplate<
          WaylandLayerShellIntegration>(5),
      m_xdgActivation(std::make_unique<wayland::xdg::XdgActivation>()) {}

WaylandLayerShellIntegration::~WaylandLayerShellIntegration() {
  if (object() && zwlr_layer_shell_v1_get_version(object()) >=
                      ZWLR_LAYER_SHELL_V1_DESTROY_SINCE_VERSION)
  {
    zwlr_layer_shell_v1_destroy(object());
  }
}

wayland::xdg::XdgActivation *WaylandLayerShellIntegration::activation() const {
  return m_xdgActivation.get();
}

QtWaylandClient::QWaylandShellSurface *
WaylandLayerShellIntegration::createShellSurface(
    QtWaylandClient::QWaylandWindow *window) {
  return new LayerSurface(this, window);
}
} // namespace ns::layershell
