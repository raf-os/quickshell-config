#include "hyprmonitor.h"

#include <algorithm>

#include <qlist.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qpoint.h>
#include <qproperty.h>
#include <qqmllist.h>

#include "hyprdefs.h"
#include "hyprland.h"
#include "hyprworkspace.h"
#include "qlisthelpers.h"
#include "workspacesmodel.h"

namespace ns::hyprland {
HyprMonitor::HyprMonitor(int id, QObject *parent) : QObject(parent), m_id(id) {
  QObject::connect(Hyprland::instance()->workspacesModel(),
      &WorkspacesModel::workspacesChanged, this,
      &HyprMonitor::onWorkspacesChanged);
  onWorkspacesChanged({});
}

void HyprMonitor::processData(common::HyprMonitorData data) {
  if (data.id != m_id) return; // invalid

  {
    QScopedPropertyUpdateGroup group;

    b_name     = data.name;
    b_model    = data.model;
    b_disabled = data.disabled;
    b_position = QPoint(data.x, data.y);
    b_size     = QPoint(data.width, data.height);

    m_activeWorkspaceData.id   = data.activeWorkspace.id;
    m_activeWorkspaceData.name = data.activeWorkspace.name;
  }

  tryAssignActiveWorkspace();
}

QQmlListProperty<HyprWorkspace> HyprMonitor::workspaces() {
  return readonlyQmlList(this, &m_workspaces);
}

HyprWorkspace *HyprMonitor::activeWorkspace() { return m_activeWorkspace; }

void HyprMonitor::tryAssignActiveWorkspace() {
  auto allWorkspaces = Hyprland::instance()->workspacesModel()->valueList();
  auto it = std::ranges::find_if(allWorkspaces.cbegin(), allWorkspaces.cend(),
      [this](HyprWorkspace *w) { return w->id() == m_activeWorkspaceData.id; });

  if (it != allWorkspaces.cend()) {
    if (m_activeWorkspace == *it) return;

    if (m_activeWorkspace) {
      QObject::disconnect(m_activeWorkspace, nullptr, this, nullptr);
    }

    m_activeWorkspace = *it;
    if (m_activeWorkspace) {
      QObject::connect(m_activeWorkspace, &QObject::destroyed, this,
          [this]() { tryAssignActiveWorkspace(); });
    }

    emit activeWorkspaceChanged();
  }
}

void HyprMonitor::onWorkspacesChanged(
    const QList<HyprWorkspace *> & /*unused*/) {
  auto changed = m_workspaces;

  for (const auto &workspace :
      Hyprland::instance()->workspacesModel()->valueList())
  {
    if (workspace->bindableMonitorId().value() != m_id) {
      changed.removeOne(workspace);
      continue;
    }

    if (changed.contains(workspace)) continue;

    changed.append(workspace);
  }

  if (m_workspaces != changed) {
    m_workspaces = changed;
    emit workspacesChanged();
  }

  tryAssignActiveWorkspace();
};
} // namespace ns::hyprland
