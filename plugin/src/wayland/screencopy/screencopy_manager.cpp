#include "screencopy_manager.h"

#include <string_view>

#include <qloggingcategory.h>
#include <qobject.h>
#include <qscreen.h>

#include "image_copy_capture/iccmanager.h"
#include "screencopy_context.h"
#include "toplevels/wl_toplevel_handle.h"

namespace ns::wayland::screencopy {
Q_LOGGING_CATEGORY(logNSScreencopy,
                   "ns.wayland.screencopy")

static const std::string_view SC_ICC_INACTIVE_MANAGER_ERROR =
    "Unable to create context - ICC manager is not active. This could "
    "be due to your compositor not supporting the wayland protocols "
    "ext-image-capture-source-v1 and/or ext-image-copy-capture-v1.";

ScreencopyContext *ScreencopyManager::createContext(QObject *object) {
  if (auto *toplevel = qobject_cast<toplevels::WLToplevelHandle *>(object)) {
    auto *manager = icc::IccManager::instance();
    if (!manager->isActive()) {
      qCWarning(logNSScreencopy) << SC_ICC_INACTIVE_MANAGER_ERROR;
      return nullptr;
    }
    auto ctx = manager->createContextFromToplevel(toplevel);
    return ctx;
  } else if (auto *screen = qobject_cast<QScreen *>(object)) {
    auto *manager = icc::IccManager::instance();
    if (!manager->isActive()) {
      qCWarning(logNSScreencopy) << SC_ICC_INACTIVE_MANAGER_ERROR;
      return nullptr;
    }
    auto ctx = manager->createContextFromToplevel(toplevel);
    return ctx;
  }

  qCWarning(logNSScreencopy)
      << "ScreencopyManager::createContext: Invalid reference object provided:"
      << object;
  return nullptr;
}
} // namespace ns::wayland::screencopy
