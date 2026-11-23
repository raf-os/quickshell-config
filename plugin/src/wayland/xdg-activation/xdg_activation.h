#pragma once

#include <optional>

#include <QtWaylandClient/QWaylandClientExtension>

#include "qwayland-xdg-activation-v1.h"

namespace QtWaylandClient {
class QWaylandDisplay;
class QWaylandSurface;
} // namespace QtWaylandClient

namespace ns::wayland::xdg {
class XdgActivationToken;

class XdgActivation : public QWaylandClientExtensionTemplate<XdgActivation>,
                      public QtWayland::xdg_activation_v1 {
public:
  explicit XdgActivation();
  ~XdgActivation() override;

  XdgActivationToken *requestXdgActivationToken(
      QtWaylandClient::QWaylandDisplay *display, struct ::wl_surface *surface,
      std::optional<uint32_t> serial, const QString &app_id);
};
} // namespace ns::wayland::xdg
