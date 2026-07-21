#include "hyprworkspace.h"

#include <qloggingcategory.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmllist.h>

#include "hyprdefs.h"
#include "qlisthelpers.h"
#include "toplevelmodel.h"

namespace ns::hyprland {
Q_DECLARE_LOGGING_CATEGORY(logNSHyprland) // from hyprland.cpp

HyprWorkspace::HyprWorkspace(int      id,
                             QObject *parent)
    : QObject(parent),
      m_id(id) {}

void HyprWorkspace::updateData(common::HyprWorkspaceData data) {
  if (data.id != m_id) return; // received invalid data

  {
    QScopedPropertyUpdateGroup group;

    if (QString::number(m_id) == data.name) b_name = "";
    else b_name = data.name;
    b_isPersistent = data.isPersistent;
    b_monitorId    = data.monitorId;
    b_monitorName  = data.monitorName;
  }
}

QQmlListProperty<ToplevelInstance> HyprWorkspace::toplevels() {
  return readonlyQmlList(this, &m_childToplevels);
}

void HyprWorkspace::attachToplevel(ToplevelInstance *toplevel) {
  auto idx = m_childToplevels.indexOf(toplevel);
  if (idx != -1) return;

  QObject::connect(toplevel,
                   &ToplevelInstance::destroyed,
                   this,
                   [this, toplevel] { this->detachToplevel(toplevel); });
  QObject::connect(toplevel,
                   &ToplevelInstance::workspaceIdChanged,
                   this,
                   [this, toplevel] { this->detachToplevel(toplevel); });

  m_childToplevels.append(toplevel);
  emit toplevelsChanged();
}

void HyprWorkspace::detachToplevel(ToplevelInstance *toplevel) {
  auto idx = m_childToplevels.indexOf(toplevel);
  if (idx == -1) return;

  if (toplevel) QObject::disconnect(toplevel, nullptr, this, nullptr);

  if (m_childToplevels.removeOne(toplevel)) {
    emit toplevelsChanged();
  }
}
} // namespace ns::hyprland
