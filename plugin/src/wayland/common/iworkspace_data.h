#pragma once

#include "compositor_support.h"

namespace ns::wayland::workspaces {
class IWorkspaceData {
public:
  virtual ~IWorkspaceData() = 0;

  virtual constexpr CompositorSupport compositor() const = 0;
};
} // namespace ns::wayland::workspaces
