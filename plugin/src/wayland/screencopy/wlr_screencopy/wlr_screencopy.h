#pragma once

#include <qscreen.h>
#include <qwayland-wlr-screencopy-unstable-v1.h>
#include <qwaylandclientextension.h>

#include "screencopy_context.h"

namespace ns::wayland::screencopy::wlr {
class WlrScreencopyManager
    : public QWaylandClientExtensionTemplate<WlrScreencopyManager>,
      public QtWayland::zwlr_screencopy_manager_v1 {
public:
  static WlrScreencopyManager *instance();

  ScreencopyContext *captureOutput(QScreen *screen);

private:
  explicit WlrScreencopyManager();
};
} // namespace ns::wayland::screencopy::wlr
