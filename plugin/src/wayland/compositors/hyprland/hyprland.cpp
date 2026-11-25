#include "hyprland.h"

#include "hyprland_p.h"

namespace ns::wayland::hyprland {
Hyprland::Hyprland(const HyprlandDependencies &dependencies, QObject *parent)
    : QObject(parent), d(std::make_unique<HyprlandPrivate>()) {
  d->workspaceManager = dependencies.workspaceManager;
}

Hyprland::~Hyprland() = default;

void Hyprland::onWorkspacesChanged() {}
} // namespace ns::wayland::hyprland
