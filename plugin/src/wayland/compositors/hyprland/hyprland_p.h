#pragma once

#include "shell_global.h"
#include "workspace_manager.h"

namespace ns::wayland::hyprland {
class HyprlandPrivate {
public:
  workspaces::WorkspaceManager *workspaceManager = nullptr;
  ns::ShellGlobal              *shellGlobal      = nullptr;
};
} // namespace ns::wayland::hyprland
