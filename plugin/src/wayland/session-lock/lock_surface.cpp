#include "lock_surface.h"

#include <private/qwaylandscreen_p.h>
#include <private/qwaylandshellsurface_p.h>
#include <private/qwaylandsurface_p.h>
#include <private/qwaylandwindow_p.h>
#include <qlogging.h>
#include <qscreen_platform.h>
#include <qsize.h>

#include "session_lock.h"
#include "window_attached_lock.h"

namespace ns::wayland::sessionlock {
LockSurface::LockSurface(QtWaylandClient::QWaylandWindow *window)
    : QtWaylandClient::QWaylandShellSurface(window) {
  auto *qwin = window->window();

  wl_output *output = nullptr;
  auto       waylandScreen =
      dynamic_cast<QtWaylandClient::QWaylandScreen *>(qwin->screen()->handle());

  if (waylandScreen == nullptr || waylandScreen->isPlaceholder() ||
      !waylandScreen->output())
  {
    qFatal() << "ns::wayland::sessionlock::LockSurface: Invalid screen.";
  }

  output = waylandScreen->output();
  QtWayland::ext_session_lock_surface_v1::init(
      m_attachedLock->m_lock->get_lock_surface(
          window->waylandSurface()->object(), output));
}

LockSurface::~LockSurface() {
  if (object() == nullptr) return;
  if (m_attachedLock != nullptr) m_attachedLock->m_surface = nullptr;
  QtWayland::ext_session_lock_surface_v1::destroy();
}

void LockSurface::setAttachment(WindowAttachedLock *lock) {
  if (!lock) {
    if (window()) window()->window()->close();
  } else {
    if (m_attachedLock) m_attachedLock->m_surface = nullptr;

    m_attachedLock            = lock;
    m_attachedLock->m_surface = this;
  }
}

void LockSurface::setVisible() { window()->window()->setVisible(true); }
bool LockSurface::commitSurfaceRole() const { return false; }

bool LockSurface::isConfigured() const { return m_configured; }
void LockSurface::applyConfigure() {
  window()->resizeFromApplyConfigure(m_size);
}

void LockSurface::ext_session_lock_surface_v1_configure(
    quint32 serial, quint32 width, quint32 height) {
  if (!window()) return;

  QtWayland::ext_session_lock_surface_v1::ack_configure(serial);

  m_size = QSize(static_cast<qint32>(width), static_cast<qint32>(height));

  m_configured = true;
  applyConfigure();
}
} // namespace ns::wayland::sessionlock
