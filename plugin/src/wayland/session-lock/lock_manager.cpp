#include "lock_manager.h"

#include <qwaylandclientextension.h>

#include "helpermacros.h"
#include "qwayland-ext-session-lock-v1.h"
#include "session_lock.h"

namespace ns::wayland::sessionlock {
AUTO_MEYERS_SINGLETON_IMPL(LockManager)

LockManager::LockManager() : QWaylandClientExtensionTemplate<LockManager>(1) {
  initialize();
}

LockManager::~LockManager() { destroy(); }

bool LockManager::isLocked() const { return m_activeLock != nullptr; }
bool LockManager::isSecure() const {
  return m_activeLock != nullptr && m_activeLock->isLocked();
}

SessionLock *LockManager::acquireLock() {
  if (isLocked()) return nullptr;
  m_activeLock =
      new SessionLock(this, QtWayland::ext_session_lock_manager_v1::lock());
  return m_activeLock;
}

SessionLock *LockManager::getLock() { return m_activeLock; }
void         LockManager::clearLock() { m_activeLock = nullptr; }
} // namespace ns::wayland::sessionlock
