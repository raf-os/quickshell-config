#include "lock_surface.h"

#include <cmath>

#include <private/qwaylandscreen_p.h>
#include <private/qwaylandshellsurface_p.h>
#include <private/qwaylandsurface_p.h>
#include <private/qwaylandwindow_p.h>
#include <qassert.h>
#include <qlogging.h>
#include <qscopedpointer.h>
#include <qscreen_platform.h>
#include <qsize.h>
#include <qtdeprecationdefinitions.h>

#include "session_lock.h"
#include "window_attached_lock.h"

namespace ns::wayland::sessionlock {
LockSurface::LockSurface(QtWaylandClient::QWaylandWindow *window)
    : QtWaylandClient::QWaylandShellSurface(window) {
  auto *qwin = window->window();
  setAttachment(WindowAttachedLock::getForWindow(qwin));

  if (m_attachedLock == nullptr) {
    qFatal() << "LockSurface created with null attachment.";
  }

  if (m_attachedLock->m_lock == nullptr) {
    qFatal() << "SessionLock for LockSurface died.";
  }

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

// void LockSurface::setVisible() {
//   if (m_configured && !m_visible) initVisible();
//   m_visible = true;
// }

bool LockSurface::isConfigured() const { return m_configured; }
bool LockSurface::isExposed() const { return m_configured; }

void LockSurface::applyConfigure() {
  window()->resizeFromApplyConfigure(m_size);
}

void LockSurface::ext_session_lock_surface_v1_configure(
    quint32 serial, quint32 width, quint32 height) {
  if (!window()) return;

  QtWayland::ext_session_lock_surface_v1::ack_configure(serial);

  m_size = QSize(static_cast<qint32>(width), static_cast<qint32>(height));

  if (!m_configured) {
    m_configured = true;
    // applyConfigure();
    window()->resizeFromApplyConfigure(m_size);
    window()->updateExposure();

    // if (m_visible) initVisible();
  } else {
    // applyConfigure();
    window()->resizeFromApplyConfigure(m_size);
  }
}

class SurfaceAccessor : public QtWaylandClient::QWaylandWindow {
public:
  QScopedPointer<QtWaylandClient::QWaylandSurface> &surfacePointer() {
    return mSurface;
  }
};

class HackSurface : public QObject, public QtWayland::wl_surface {
public:
  HackSurface(QtWaylandClient::QWaylandDisplay *display)
      : wl_surface(display->createSurface(this)) {}
  ~HackSurface() override { this->destroy(); }
  Q_DISABLE_COPY_MOVE(HackSurface);
};

// void LockSurface::initVisible() {
//   m_visible = true;
//
//   auto *dummySurface = new HackSurface(window()->display());
//   auto *tempSurface  = new QScopedPointer(
//       reinterpret_cast<QtWaylandClient::QWaylandSurface *>(dummySurface));
//
//   auto &surfacePointer =
//       reinterpret_cast<SurfaceAccessor *>(window())->surfacePointer();
//
//   QT_WARNING_PUSH
//   QT_WARNING_DISABLE_DEPRECATED {
//     surfacePointer.swap(*tempSurface);
//     window()->window()->setVisible(true);
//     surfacePointer.swap(*tempSurface);
//   }
//   QT_WARNING_POP
//
//   delete reinterpret_cast<QScopedPointer<HackSurface> *>(tempSurface);
//   if (surfacePointer->version() >= 3) {
//     surfacePointer->set_buffer_scale(std::ceil(window()->scale()));
//   }
// }
} // namespace ns::wayland::sessionlock
