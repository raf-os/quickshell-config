#pragma once

#include <qtclasshelpermacros.h>
#include <qwayland-ext-session-lock-v1.h>
#include <qwaylandclientextension.h>

#include "helpermacros.h"
#include "session_lock.h"

namespace ns::wayland::sessionlock {
class LockManager : public QWaylandClientExtensionTemplate<LockManager>,
                    public QtWayland::ext_session_lock_manager_v1 {
  AUTO_MEYERS_SINGLETON_DECL(LockManager)
public:
  ~LockManager() override;

  SessionLock       *acquireLock();
  [[nodiscard]] bool isLocked() const;
  [[nodiscard]] bool isSecure() const;

  [[nodiscard]] SessionLock *getLock();
  void                       clearLock();

public slots:
  bool attemptLock();
  bool attemptUnlock();

private:
  explicit LockManager();
  SessionLock *m_activeLock = nullptr;
};
} // namespace ns::wayland::sessionlock
