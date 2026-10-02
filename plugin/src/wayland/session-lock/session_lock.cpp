#include "session_lock.h"

#include "lock_manager.h"
#include "wayland-ext-session-lock-v1-client-protocol.h"

namespace ns::wayland::sessionlock {
SessionLock::SessionLock(LockManager *manager, ::ext_session_lock_v1 *lock)
    : m_manager(manager) {
  QtWayland::ext_session_lock_v1::init(lock);
}

SessionLock::~SessionLock() {
  // Warning: destroying this object without unlocking it first will brick the
  // current session, as the protocol dictates
  if (isInitialized()) QtWayland::ext_session_lock_v1::destroy();
}

bool SessionLock::isLocked() const { return m_isSecure; }
bool SessionLock::isActive() const { return isInitialized(); }

void SessionLock::unlock() {
  if (isInitialized()) {
    if (m_isFinished) QtWayland::ext_session_lock_v1::destroy();
    else QtWayland::ext_session_lock_v1::unlock_and_destroy();

    m_isSecure = false;
    m_manager->clearLock();

    emit unlocked();
  }
}

void SessionLock::ext_session_lock_v1_locked() {
  m_isSecure = true;
  emit locked();
}
void SessionLock::ext_session_lock_v1_finished() {
  m_isSecure   = false;
  m_isFinished = true;
  emit unlocked();
}
} // namespace ns::wayland::sessionlock
