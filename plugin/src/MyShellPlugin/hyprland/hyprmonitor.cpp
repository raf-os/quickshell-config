#include "hyprmonitor.h"

#include <qlist.h>
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
HyprMonitor::HyprMonitor(int      id,
                         QObject *parent)
    : QObject(parent),
      m_id(id) {
  QObject::connect(Hyprland::instance()->workspacesModel(),
                   &WorkspacesModel::workspacesChanged,
                   this,
                   &HyprMonitor::onWorkspacesChanged);
}

void HyprMonitor::processData(common::HyprMonitorData data) {
  if (data.id != m_id) return; // invalid

  {
    QScopedPropertyUpdateGroup group;

    m_name     = data.name;
    m_model    = data.model;
    m_disabled = data.disabled;
    m_position = QPoint(data.x, data.y);
    m_size     = QPoint(data.width, data.height);
  }
}

QQmlListProperty<HyprWorkspace> HyprMonitor::workspaces() {
  return readonlyQmlList(this, &m_workspaces);
}
void HyprMonitor::onWorkspacesChanged(
    const QList<HyprWorkspace *> &newWorkspaces) {
  auto changed = m_workspaces;

  for (const auto &workspace : newWorkspaces) {
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
};
} // namespace ns::hyprland
