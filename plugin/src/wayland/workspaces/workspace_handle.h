#pragma once

#include <cstdint>

#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

#include "compositor_support.h"
#include "iworkspace_data.h"
#include "ns_wayland_workspaces_export.h"
#include "qwayland-ext-workspace-v1.h"

namespace ns::wayland::workspaces {
namespace WorkspaceState {
Q_NAMESPACE_EXPORT(NS_WAYLAND_WORKSPACES_EXPORT)

enum Enum : uint32_t {
  Error  = 0,
  Active = 1 << 0,
  Urgent = 1 << 1,
  Hidden = 1 << 2
};

Q_ENUM_NS(Enum)
QML_NAMED_ELEMENT(WorkspaceState)
}; // namespace WorkspaceState

namespace WorkspaceCapabilities {
Q_NAMESPACE_EXPORT(NS_WAYLAND_WORKSPACES_EXPORT)

enum Enum : uint32_t {
  None       = 0,
  Activate   = 1 << 0,
  Deactivate = 1 << 1,
  Remove     = 1 << 2,
  Assign     = 1 << 3
};

Q_ENUM_NS(Enum)
QML_NAMED_ELEMENT(WorkspaceCapabilities)
}; // namespace WorkspaceCapabilities

class NS_WAYLAND_WORKSPACES_EXPORT WorkspaceHandle
    : public QObject,
      public QtWayland::ext_workspace_handle_v1 {
  Q_OBJECT

public:
  explicit WorkspaceHandle(
      CompositorData *compositorData, ::ext_workspace_handle_v1 *object);

  [[nodiscard]] QBindable<QString>              bindableId() const;
  [[nodiscard]] QBindable<QString>              bindableName() const;
  [[nodiscard]] QBindable<WorkspaceState::Enum> bindableState() const;
  [[nodiscard]] QBindable<WorkspaceCapabilities::Enum>
  bindableCapabilities() const;

  void ext_workspace_handle_v1_id(const QString &id) override;
  void ext_workspace_handle_v1_name(const QString &name) override;
  void ext_workspace_handle_v1_coordinates(wl_array *coordinates) override;
  void ext_workspace_handle_v1_state(uint32_t state) override;
  void ext_workspace_handle_v1_capabilities(uint32_t capabilities) override;
  void ext_workspace_handle_v1_removed() override;

signals:
  void destroying();
  void idChanged();
  void nameChanged();
  void stateChanged();
  void capabilitiesChanged();

private:
  CompositorData *m_compositorData;

  Q_OBJECT_BINDABLE_PROPERTY(
      WorkspaceHandle, QString, b_id, &WorkspaceHandle::idChanged)
  Q_OBJECT_BINDABLE_PROPERTY(
      WorkspaceHandle, QString, b_name, &WorkspaceHandle::nameChanged)
  Q_OBJECT_BINDABLE_PROPERTY(WorkspaceHandle, WorkspaceState::Enum, b_state,
      &WorkspaceHandle::stateChanged)
  Q_OBJECT_BINDABLE_PROPERTY(WorkspaceHandle, WorkspaceCapabilities::Enum,
      b_capabilities, &WorkspaceHandle::capabilitiesChanged)

  IWorkspaceData *m_data = nullptr;
};
} // namespace ns::wayland::workspaces
