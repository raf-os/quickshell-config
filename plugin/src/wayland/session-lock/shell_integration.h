#pragma once

#include <private/qwaylanddisplay_p.h>
#include <private/qwaylandshellintegration_p.h>
#include <private/qwaylandshellsurface_p.h>
#include <private/qwaylandwindow_p.h>

namespace ns::wayland::sessionlock {
class SessionLockShellIntegration
    : public QtWaylandClient::QWaylandShellIntegration {
public:
  bool initialize(QtWaylandClient::QWaylandDisplay * /*unused*/) override {
    return true;
  }
  QtWaylandClient::QWaylandShellSurface *createShellSurface(
      QtWaylandClient::QWaylandWindow *window) override;
};
} // namespace ns::wayland::sessionlock
