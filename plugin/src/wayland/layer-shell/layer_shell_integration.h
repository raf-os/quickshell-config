#pragma once

#include <memory>

#include <QtWaylandClient/private/qwaylandshellintegration_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>
#include <private/qwaylandshellsurface_p.h>

#include "ns_layershell_export.h"
#include "qwayland-wlr-layer-shell-unstable-v1.h"

namespace ns::wayland::xdg {
class XdgActivation;
}

namespace ns::layershell {
class NS_LAYERSHELL_EXPORT WaylandLayerShellIntegration
    : public QtWaylandClient::QWaylandShellIntegrationTemplate<
          WaylandLayerShellIntegration>,
      public QtWayland::zwlr_layer_shell_v1 {
public:
  explicit WaylandLayerShellIntegration();
  ~WaylandLayerShellIntegration() override;
  Q_DISABLE_COPY_MOVE(WaylandLayerShellIntegration)

  wayland::xdg::XdgActivation *activation() const;

  QtWaylandClient::QWaylandShellSurface *
  createShellSurface(QtWaylandClient::QWaylandWindow *window) override;

private:
  std::unique_ptr<wayland::xdg::XdgActivation> m_xdgActivation;
};
}; // namespace ns::layershell
