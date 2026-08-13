#include "screencopy_manager.h"

#include <qloggingcategory.h>
#include <qobject.h>

#include "image_copy_capture/iccmanager.h"
#include "screencopy_context.h"
#include "toplevels/wl_toplevel_handle.h"

namespace ns::wayland::screencopy {
Q_LOGGING_CATEGORY(logNSScreencopy,
                   "ns.wayland.screencopy")

ScreencopyContext *ScreencopyManager::createContext(QObject *object) {
  if (auto *toplevel = qobject_cast<toplevels::WLToplevelHandle *>(object)) {
    auto *mainManager = icc::IccManager::instance();
    auto *manager     = icc::IccForeignToplevelManager::instance();
    if (mainManager->isActive() && manager->isActive()) {
      auto source = manager->createSource(toplevel);
      auto ctx    = mainManager->createSession(source);

      return ctx;
    } else {
      qCWarning(logNSScreencopy)
          << "Unable to create context - ICC managers are not active. This "
             "could be because your compositor does not support "
             "ext-image-capture-source-v1 and/or ext-image-copy-capture-v1.";
      return nullptr;
    }
  }

  return nullptr;
}
} // namespace ns::wayland::screencopy
