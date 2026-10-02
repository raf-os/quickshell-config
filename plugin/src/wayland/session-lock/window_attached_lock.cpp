#include "window_attached_lock.h"

#include <private/qwaylandwindow_p.h>
#include <qdebug.h>
#include <qlogging.h>
#include <qobject.h>
#include <qvariant.h>
#include <qwindow.h>

#include "lock_manager.h"
#include "lock_surface.h"
#include "shell_integration.h"

namespace ns::wayland::sessionlock {
WindowAttachedLock::WindowAttachedLock(QObject *parent) : QObject(parent) {}

WindowAttachedLock::~WindowAttachedLock() {
  if (m_surface) m_surface->setAttachment(nullptr);
}

LockSurface *WindowAttachedLock::getSurface() { return m_surface; }

WindowAttachedLock *WindowAttachedLock::getForWindow(QWindow *window) {
  auto v = window->property("sessionlock_attached");

  if (v.canConvert<WindowAttachedLock *>()) {
    return v.value<WindowAttachedLock *>();
  } else {
    return nullptr;
  }
}

bool WindowAttachedLock::isAttached() const { return m_surface != nullptr; }

bool WindowAttachedLock::attach(QWindow *window) {
  if (m_surface != nullptr) {
    qFatal() << "Tried to change the attached window of a WindowAttachedLock "
                "instance.";
  }

  auto *current = WindowAttachedLock::getForWindow(window);
  QtWaylandClient::QWaylandWindow *waylandWindow = nullptr;

  if (current) {
    current->getSurface()->setAttachment(this);
  } else {
    auto *screen = window->screen();
    window->create();
    window->setScreen(screen);

    waylandWindow =
        dynamic_cast<QtWaylandClient::QWaylandWindow *>(window->handle());
    if (!waylandWindow) {
      qWarning() << "Attempted attaching" << this << "to" << window
                 << "but said window is not a wayland one.";
      return false;
    }

    static SessionLockShellIntegration *lockIntegration = nullptr;
    if (lockIntegration == nullptr) {
      lockIntegration = new SessionLockShellIntegration();
      if (!lockIntegration->initialize(waylandWindow->display())) {
        delete lockIntegration;
        lockIntegration = nullptr;
        qWarning() << "Unable to initialize lockscreen shell integration.";
      }
    }

    waylandWindow->setShellIntegration(lockIntegration);
  }

  this->setParent(window);
  window->setProperty("sessionlock_attached", QVariant::fromValue(this));
  m_lock = LockManager::instance()->getLock();

  if (waylandWindow) {
    m_surface = new LockSurface(waylandWindow);
    if (m_pendingVisibility) m_surface->setVisible();
  }

  return true;
}

void WindowAttachedLock::setVisible() {
  if (!m_surface) m_pendingVisibility = true;
  else m_surface->setVisible();
}
} // namespace ns::wayland::sessionlock
