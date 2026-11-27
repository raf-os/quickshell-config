#include "workspace_model.h"

#include "workspace_manager.h"

namespace ns {
class WorkspaceModelPrivate {
public:
  wayland::workspaces::WorkspaceManager *manager = nullptr;
};

WorkspaceModel::WorkspaceModel(QObject *parent)
    : QAbstractItemModel(parent), d(std::make_unique<WorkspaceModelPrivate>()) {
}

WorkspaceModel::~WorkspaceModel() = default;

ShellGlobal *WorkspaceModel::shellGlobal() { return m_shellGlobal; }

void WorkspaceModel::setShellGlobal(ShellGlobal *value) {
  m_shellGlobal = value;

  if (value) {
    d->manager = value->workspaceManager();
  }
}

int WorkspaceModel::rowCount(const QModelIndex &index) const { return 0; }

int WorkspaceModel::columnCount(const QModelIndex &index) const { return 0; }

QHash<int, QByteArray> WorkspaceModel::roleNames() const {}

QModelIndex WorkspaceModel::index(int row, int column,
                                  const QModelIndex &parent) const {}

QModelIndex WorkspaceModel::parent(const QModelIndex &index) const {}

QVariant WorkspaceModel::data(const QModelIndex &index, int role) const {}

} // namespace ns
