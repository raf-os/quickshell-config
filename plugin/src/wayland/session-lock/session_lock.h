#pragma once

#include <qobject.h>
#include <qtclasshelpermacros.h>
#include <qtmetamacros.h>

#include "qwayland-ext-session-lock-v1.h"
#include "wayland-ext-session-lock-v1-client-protocol.h"

namespace ns::wayland::sessionlock {
class LockManager;

class SessionLock : public QObject, public QtWayland::ext_session_lock_v1 {
  Q_OBJECT

public:
  SessionLock(LockManager *manager, ::ext_session_lock_v1 *lock);
  ~SessionLock() override;
  Q_DISABLE_COPY_MOVE(SessionLock)

  [[nodiscard]] bool isActive() const;
  [[nodiscard]] bool isLocked() const;
  void               unlock();

signals:
  void locked();
  void unlocked();

private:
  void ext_session_lock_v1_locked() override;
  void ext_session_lock_v1_finished() override;

  bool         m_isSecure   = false;
  bool         m_isFinished = false;
  LockManager *m_manager;
};
} // namespace ns::wayland::sessionlock
