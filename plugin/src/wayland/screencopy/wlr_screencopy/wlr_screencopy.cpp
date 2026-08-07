#include "wlr_screencopy.h"

#include <private/qwaylanddisplay_p.h>
#include <private/qwaylandscreen_p.h>
#include <qloggingcategory.h>
#include <qscreen.h>
#include <qwaylandclientextension.h>
#include <wayland-wlr-screencopy-unstable-v1-client-protocol.h>

#include "screencopy_context.h"
#include "wlr_screencopy_context.h"

namespace ns::wayland::screencopy::wlr {
Q_LOGGING_CATEGORY(logNSScreencopyWlr,
                   "nightshell.wayland.screencopy.wlr")

WlrScreencopyManager::WlrScreencopyManager()
    : QWaylandClientExtensionTemplate(3) {
  this->initialize();
}

WlrScreencopyManager *WlrScreencopyManager::instance() {
  static auto s_instance = new WlrScreencopyManager();
  return s_instance;
}

ScreencopyContext *WlrScreencopyManager::captureOutput(QScreen *screen) {
  if (!dynamic_cast<QtWaylandClient::QWaylandScreen *>(screen->handle()))
    return nullptr;

  return new WlrScreencopyContext(this, screen);
}
} // namespace ns::wayland::screencopy::wlr
