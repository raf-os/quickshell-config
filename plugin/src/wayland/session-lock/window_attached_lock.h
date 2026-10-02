#pragma once

#include <qobject.h>
#include <qtclasshelpermacros.h>
#include <qtmetamacros.h>
#include <qwindow.h>

namespace ns::wayland::sessionlock {
class LockSurface;
class SessionLock;

class WindowAttachedLock : public QObject {
  Q_OBJECT

public:
  explicit WindowAttachedLock(QObject *parent = nullptr);
  ~WindowAttachedLock() override;
  Q_DISABLE_COPY_MOVE(WindowAttachedLock)

  bool               attach(QWindow *window);
  [[nodiscard]] bool isAttached() const;

  void setVisible();

  static WindowAttachedLock *getForWindow(QWindow *window);
  [[nodiscard]] LockSurface *getSurface();

private:
  bool         m_pendingVisibility = false;
  LockSurface *m_surface           = nullptr;
  SessionLock *m_lock              = nullptr;

  friend class LockSurface;
};
} // namespace ns::wayland::sessionlock
