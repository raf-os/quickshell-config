#pragma once

#include "wl_toplevel_handle.h"

namespace ns::wayland::toplevels {
class ToplevelForeignHandler {
  virtual void onWlrMapRequest(WLToplevelHandle *handle) = 0;

  virtual ~ToplevelForeignHandler() {}
};
} // namespace ns::wayland::toplevels
