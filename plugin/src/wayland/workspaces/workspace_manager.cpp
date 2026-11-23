#include "workspace_manager.h"

#include <qloggingcategory.h>
#include <qobject.h>
#include <qwaylandclientextension.h>

#include "compositor_support.h"
#include "workspace_handle.h"

namespace ns::wayland::workspaces {
WorkspaceManager::WorkspaceManager(
    CompositorData *compositorData, QObject *parent)
    : QWaylandClientExtensionTemplate(1, parent),
      m_compositorData(compositorData) {
  initialize();
}

QHash<QString, WorkspaceHandle *> WorkspaceManager::workspaces() const {
  return m_readyWorkspaces;
}

void WorkspaceManager::ext_workspace_manager_v1_workspace(
    ::ext_workspace_handle_v1 *handle) {
  auto _handle = new WorkspaceHandle(m_compositorData, handle);

  QObject::connect(_handle, &QObject::destroyed, this,
      &WorkspaceManager::onWorkspaceDestroyed);
  QObject::connect(_handle, &WorkspaceHandle::destroying, this,
      &WorkspaceManager::onWorkspaceDestroyed);

  m_stagingWorkspaces.append(_handle);
}

void WorkspaceManager::ext_workspace_manager_v1_workspace_group(
    ::ext_workspace_group_handle_v1 *handle) {}

void WorkspaceManager::ext_workspace_manager_v1_done() {
  if (m_stagingWorkspaces.size() > 0) {
    for (auto workspace : m_stagingWorkspaces) {
      m_readyWorkspaces.insert(workspace->bindableName().value(), workspace);
    }
    m_stagingWorkspaces.clear();
    emit workspacesChanged();
  }
}

void WorkspaceManager::ext_workspace_manager_v1_finished() {
  for (auto [key, value] : m_readyWorkspaces.asKeyValueRange()) {
    value->deleteLater();
  }
  m_readyWorkspaces.clear();

  for (auto workspace : m_stagingWorkspaces) {
    workspace->deleteLater();
  }
  m_stagingWorkspaces.clear();

  emit workspacesChanged();
}

void WorkspaceManager::onWorkspaceDestroyed() {
  auto workspace = static_cast<WorkspaceHandle *>(sender());

  if (workspace) {
    m_stagingWorkspaces.removeOne(workspace);
    m_readyWorkspaces.remove(workspace->bindableName().value());
  }
}
} // namespace ns::wayland::workspaces
