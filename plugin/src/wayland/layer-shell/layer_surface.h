#pragma once

#include <cstdint>

#include <QMargins>
#include <QSize>
#include <QtTypes>
#include <QtWaylandClient/private/qwaylandshellsurface_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>
#include <wayland-client.h>

#include "layer_shell_integration.h"
#include "ns_layershell_export.h"
#include "qwayland-wlr-layer-shell-unstable-v1.h"

namespace ns::layershell {
class LayerWindow;

class NS_LAYERSHELL_EXPORT LayerSurface
    : public QtWaylandClient::QWaylandShellSurface,
      public QtWayland::zwlr_layer_surface_v1 {
  Q_OBJECT

public:
  explicit LayerSurface(WaylandLayerShellIntegration    *shell,
                        QtWaylandClient::QWaylandWindow *window);
  ~LayerSurface() override;
  Q_DISABLE_COPY_MOVE(LayerSurface)

  [[nodiscard]] bool isExposed() const override;
  void attachPopup(QtWaylandClient::QWaylandShellSurface *popup) override;

  void applyConfigure() override;
  void setWindowSize(const QSize &size) override;

  void setAnchor(uint anchor);
  void setDesiredSize(const QSize &size);
  void setExclusiveZone(int32_t zone);
  void setExclusiveEdge(uint32_t edge);
  void setMargins(const QMargins &margins);
  void setKeyboardInteractivity(uint32_t interactivity);
  void setLayer(uint32_t layer);

  bool requestActivate() override;
  bool requestActivateOnShow() override;
  void setXdgActivationToken(const QString &token) override;
  void requestXdgActivationToken(quint32 serial) override;

private:
  void zwlr_layer_surface_v1_configure(uint32_t serial, uint32_t width,
                                       uint32_t height) override;
  void zwlr_layer_surface_v1_closed() override;
  void sendExpose();

  WaylandLayerShellIntegration    *m_shell;
  LayerWindow                     *m_interface;
  QtWaylandClient::QWaylandWindow *m_window;
  QSize                            m_pendingSize;
  QString                          m_activationToken;

  bool m_configured = false;
};
} // namespace ns::layershell
