#include "workspace_handle.h"

#include "wayland-ext-workspace-v1-client-protocol.h"

namespace ns::wayland::workspaces {
WorkspaceHandle::WorkspaceHandle(
    CompositorData *compositorData, ::ext_workspace_handle_v1 *object)
    : QtWayland::ext_workspace_handle_v1(object),
      m_compositorData(compositorData) {}

void WorkspaceHandle::ext_workspace_handle_v1_id(const QString &id) {
  b_id = id;
}

void WorkspaceHandle::ext_workspace_handle_v1_name(const QString &name) {
  b_name = name;
}

void WorkspaceHandle::ext_workspace_handle_v1_coordinates(
    wl_array *coordinates) {}

void WorkspaceHandle::ext_workspace_handle_v1_state(uint32_t state) {
  b_state = static_cast<WorkspaceState::Enum>(state);
}

void WorkspaceHandle::ext_workspace_handle_v1_capabilities(
    uint32_t capabilities) {
  b_capabilities = static_cast<WorkspaceCapabilities::Enum>(capabilities);
}

void WorkspaceHandle::ext_workspace_handle_v1_removed() {
  emit destroying();

  deleteLater();
}
} // namespace ns::wayland::workspaces
