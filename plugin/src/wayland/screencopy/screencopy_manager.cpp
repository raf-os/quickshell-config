#include "screencopy_manager.h"

#include <qobject.h>

#include "image_copy_capture/iccmanager.h"
#include "screencopy_context.h"
#include "toplevels/wl_toplevel_handle.h"

namespace ns::wayland::screencopy {
ScreencopyContext *ScreencopyManager::createContext(QObject *object) {
  if (auto *toplevel = qobject_cast<toplevels::WLToplevelHandle *>(object)) {
    auto *mainManager = icc::IccManager::instance();
    auto *manager     = icc::IccForeignToplevelManager::instance();
    if (mainManager->isActive() && manager->isActive()) {
      auto source = manager->createSource(toplevel);
      auto ctx    = mainManager->createSession(source.get()->object());
    }
  }
}
} // namespace ns::wayland::screencopy
