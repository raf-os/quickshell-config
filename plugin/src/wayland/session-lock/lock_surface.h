#pragma once

#include <private/qwaylandshellsurface_p.h>
#include <private/qwaylandwindow_p.h>
#include <qsize.h>
#include <qtclasshelpermacros.h>
#include <qtypes.h>

#include "qwayland-ext-session-lock-v1.h"
#include "window_attached_lock.h"

namespace ns::wayland::sessionlock {
class LockSurface : public QtWaylandClient::QWaylandShellSurface,
                    public QtWayland::ext_session_lock_surface_v1 {
public:
  LockSurface(QtWaylandClient::QWaylandWindow *window);
  ~LockSurface() override;
  Q_DISABLE_COPY_MOVE(LockSurface)

  [[nodiscard]] bool isConfigured() const;
  void               applyConfigure() override;

  [[nodiscard]] bool commitSurfaceRole() const override;

  void setAttachment(WindowAttachedLock *att);
  void setVisible();

private:
  void ext_session_lock_surface_v1_configure(
      quint32 serial, quint32 width, quint32 height) override;

  bool                m_configured = false;
  QSize               m_size;
  WindowAttachedLock *m_attachedLock = nullptr;
};
} // namespace ns::wayland::sessionlock
