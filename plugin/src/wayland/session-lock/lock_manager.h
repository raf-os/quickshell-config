#pragma once

#include <qtclasshelpermacros.h>
#include <qtmetamacros.h>
#include <qwayland-ext-session-lock-v1.h>
#include <qwaylandclientextension.h>

#include "helpermacros.h"
#include "session_lock.h"

namespace ns::wayland::sessionlock {
class LockManager : public QWaylandClientExtensionTemplate<LockManager>,
                    public QtWayland::ext_session_lock_manager_v1 {
  Q_OBJECT

  AUTO_MEYERS_SINGLETON_DECL(LockManager)

public:
  ~LockManager() override;
  Q_DISABLE_COPY_MOVE(LockManager)

  SessionLock       *acquireLock();
  [[nodiscard]] bool isLocked() const;

  static bool sessionLocked();
  static bool isSecure();

  [[nodiscard]] SessionLock *getLock();
  void                       clearLock();

public slots:
  bool attemptLock();
  bool attemptUnlock();

signals:
  void isSecureChanged();

private:
  explicit LockManager();
  SessionLock *m_activeLock = nullptr;
};
} // namespace ns::wayland::sessionlock
