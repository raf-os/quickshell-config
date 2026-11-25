#pragma once

#include "workspace_manager.h"

namespace ns::wayland::hyprland {
class HyprlandPrivate {
public:
  workspaces::WorkspaceManager *workspaceManager = nullptr;
};
} // namespace ns::wayland::hyprland
