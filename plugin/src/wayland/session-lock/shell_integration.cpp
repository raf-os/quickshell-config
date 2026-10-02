#include "shell_integration.h"

#include <qlogging.h>

#include "lock_surface.h"
#include "window_attached_lock.h"

namespace ns::wayland::sessionlock {
QtWaylandClient::QWaylandShellSurface *
SessionLockShellIntegration::createShellSurface(
    QtWaylandClient::QWaylandWindow *window) {
  auto *lock = WindowAttachedLock::getForWindow(window->window());
  if (!lock || !lock->getSurface()) {
    qFatal() << "Tried creating shell surface on a window that was not set to "
                "visible";
  }

  return lock->getSurface();
}
} // namespace ns::wayland::sessionlock
