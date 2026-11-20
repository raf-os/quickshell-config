#include "workspaces_model.h"

#include <qabstractitemmodel.h>
#include <qobject.h>

#include "workspace_manager.h"

namespace ns::wayland::workspaces {
WorkspacesModel::WorkspacesModel(
    WorkspaceManager *workspaceManager, QObject *parent)
    : QAbstractListModel(parent), m_manager(workspaceManager) {
  QObject::connect(workspaceManager, &WorkspaceManager::workspacesChanged, this,
      &WorkspacesModel::onWorkspacesChanged);
}

QHash<int, QByteArray> WorkspacesModel::roleNames() const {
  return {
      {Roles::ModelData, "modelData"},
  };
}

int WorkspacesModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  return static_cast<int>(m_workspaces.size());
}

QVariant WorkspacesModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) return {};

  return {};
}

void WorkspacesModel::onWorkspacesChanged() {}
} // namespace ns::wayland::workspaces
