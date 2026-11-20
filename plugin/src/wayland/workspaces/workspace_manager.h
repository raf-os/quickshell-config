#pragma once

#include <qhash.h>
#include <qlist.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwaylandclientextension.h>

#include "ns_wayland_workspaces_export.h"
#include "qwayland-ext-workspace-v1.h"
#include "wayland-ext-workspace-v1-client-protocol.h"
#include "workspace_handle.h"

namespace ns::wayland::workspaces {
class WorkspaceHandle;

class NS_WAYLAND_WORKSPACES_EXPORT WorkspaceManager
    : public QWaylandClientExtensionTemplate<WorkspaceManager>,
      public QtWayland::ext_workspace_manager_v1 {
  Q_OBJECT

public:
  explicit WorkspaceManager(
      CompositorData *compositorData, QObject *parent = nullptr);

  [[nodiscard]] QHash<QString, WorkspaceHandle *> workspaces() const;

  void ext_workspace_manager_v1_workspace(
      ::ext_workspace_handle_v1 *handle) override;
  void ext_workspace_manager_v1_workspace_group(
      ::ext_workspace_group_handle_v1 *handle) override;
  void ext_workspace_manager_v1_done() override;
  void ext_workspace_manager_v1_finished() override;

signals:
  void workspacesChanged();

private slots:
  void onWorkspaceDestroyed();

private:
  CompositorData *m_compositorData;

  QList<WorkspaceHandle *>          m_stagingWorkspaces;
  QHash<QString, WorkspaceHandle *> m_readyWorkspaces;
};
} // namespace ns::wayland::workspaces
