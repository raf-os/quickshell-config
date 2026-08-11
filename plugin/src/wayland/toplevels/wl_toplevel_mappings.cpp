#include "wl_toplevel_mappings.h"

#include <qobject.h>

#include "wl_toplevel_handle.h"
#include "wl_toplevel_manager.h"

namespace ns::wayland::toplevels {
WlToplevelMappings::WlToplevelMappings() {
  const auto managerInstance = WLToplevelManager::instance();

  for (const auto &toplevel : managerInstance->toplevels()) {
    this->onWaylandHandleReady(toplevel);
  }

  QObject::connect(managerInstance,
                   &WLToplevelManager::toplevelReady,
                   this,
                   &WlToplevelMappings::onWaylandHandleReady);
  QObject::connect(managerInstance,
                   &WLToplevelManager::toplevelClosed,
                   this,
                   &WlToplevelMappings::onWaylandHandleClosed);
}

WlToplevelMappings *WlToplevelMappings::instance() {
  static auto s_instance = new WlToplevelMappings();
  return s_instance;
}

void WlToplevelMappings::onWaylandHandleReady(WLToplevelHandle *handle) {}

void WlToplevelMappings::onWaylandHandleClosed(WLToplevelHandle *handle) {}
} // namespace ns::wayland::toplevels
