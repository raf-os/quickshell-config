#include "qml_session_lock.h"

#include <private/qwaylandscreen_p.h>
#include <qguiapplication.h>
#include <qloggingcategory.h>
#include <qmap.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlcomponent.h>
#include <qqmlengine.h>
#include <qscopeguard.h>
#include <qscreen.h>

#include "lock_manager.h"
#include "qml_lock_surface.h"
#include "wayland_logging.h"

namespace ns::wayland {
SessionLockQML::SessionLockQML(QObject *parent) : QObject(parent) {}

QBindable<bool> SessionLockQML::bindableIsLocked() const { return &b_isLocked; }
QBindable<bool> SessionLockQML::bindableIsSecure() const { return &b_isSecure; }

QQmlComponent *SessionLockQML::surfaceComponent() const {
  return m_surfaceComponent;
}
void SessionLockQML::setSurfaceComponent(QQmlComponent *component) {
  auto manager = sessionlock::LockManager::instance();

  if (manager->isLocked()) {
    qCCritical(logNSWayland) << "Attempted to change a surface component on a "
                                "SessionLockQML after the lock was active.";
    return;
  }

  if (m_surfaceComponent) m_surfaceComponent->deleteLater();
  if (component) component->setParent(this);

  m_surfaceComponent = component;
  emit surfaceComponentChanged();
}

void SessionLockQML::realizeLockTarget() {
  if (m_isLocking) {
    m_awaitingLock = true;
    return;
  }

  m_isLocking  = true;
  auto manager = sessionlock::LockManager::instance();

  auto guard = qScopeGuard([this] {
    m_isLocking = false;
    if (m_awaitingLock) {
      m_awaitingLock = false;
      realizeLockTarget();
    }
  });

  if (m_lockTarget) {
    if (!manager->isActive()) {
      qCCritical(logNSWayland)
          << "Unable to start session lock. Current compositor likely does not "
             "support the ext-session-lock-v1 wayland protocol.";
      doUnlock();
      return;
    }

    if (!m_surfaceComponent) {
      qCWarning(logNSWayland)
          << "Attempted locking screen without setting a surface. Aborting.";
      doUnlock();
      return;
    }

    updateSurfaces(false);
    if (!manager->attemptLock()) {
      qCWarning(logNSWayland) << "Unable to acquire session lock. Aborting.";
      doUnlock();
      return;
    }

    updateSurfaces(true);
    b_isLocked = manager->isLocked();
  } else {
    doUnlock();
  }
}

void SessionLockQML::lock() {
  if (m_lockTarget) return;
  m_lockTarget = true;
  realizeLockTarget();
}

void SessionLockQML::unlock() {
  if (!m_lockTarget) return;
  m_lockTarget = false;
  realizeLockTarget();
}

void SessionLockQML::doUnlock() {
  m_lockTarget = false;
  for (auto *surface : m_surfaces) {
    surface->deleteLater();
  }

  m_surfaces.clear();

  auto manager = sessionlock::LockManager::instance();
  manager->attemptUnlock();
  b_isLocked = manager->isLocked();
}

void SessionLockQML::updateSurfaces(bool show) {
  auto screens = QGuiApplication::screens();

  screens.removeIf([](QScreen *screen) {
    auto *waylandScreen =
        dynamic_cast<QtWaylandClient::QWaylandScreen *>(screen->handle());
    if (waylandScreen && !waylandScreen->isPlaceholder() &&
        waylandScreen->output())
      return true;
    return false;
  });

  auto map = m_surfaces;
  map.removeIf(
      [&screens, this](QMap<QScreen *, LockSurfaceQML *>::iterator it) {
        if (!screens.contains(it.key())) {
          it.value()->deleteLater();
          return true;
        } else return false;
      });

  for (auto *screen : screens) {
    if (!m_surfaces.contains(screen)) {
      auto *instanceObj = m_surfaceComponent->create(
          QQmlEngine::contextForObject(m_surfaceComponent));
      auto *instance = qobject_cast<LockSurfaceQML *>(instanceObj);

      if (!instance) {
        qCWarning(logNSWayland)
            << "Provided surface is not a LockSurfaceQML. Aborting.";
        if (instanceObj) instanceObj->deleteLater();
        doUnlock();
        return;
      }

      instance->setParent(this);
      instance->setScreen(screen);

      m_surfaces[screen] = instance;
    }

    if (show) {
      if (!sessionlock::LockManager::instance()->isLocked()) {
        qCFatal(logNSWayland)
            << "Attempted showing lockscreen surfaces without an active lock.";
      }

      for (auto *surface : m_surfaces.values()) {
        surface->show();
      }
    }
  }
}

void SessionLockQML::onScreensChanged() {
  if (b_isLocked.value() && !m_isLocking) {
    updateSurfaces(true);
  }
}
} // namespace ns::wayland
