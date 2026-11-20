#pragma once

#include <qabstractitemmodel.h>
#include <qhash.h>
#include <qhashfunctions.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qvariant.h>

#include "workspace_handle.h"

namespace ns::wayland::workspaces {
class WorkspaceManager;

class WorkspacesModel : public QAbstractListModel {
  Q_OBJECT

public:
  explicit WorkspacesModel(
      WorkspaceManager *workspaceManager, QObject *parent = nullptr);

  enum Roles { ModelData = Qt::UserRole };

  QHash<int, QByteArray> roleNames() const override;
  int                    rowCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;

private slots:
  void onWorkspacesChanged();

private:
  WorkspaceManager *m_manager;

  QList<WorkspaceHandle *> m_workspaces;
};
} // namespace ns::wayland::workspaces
