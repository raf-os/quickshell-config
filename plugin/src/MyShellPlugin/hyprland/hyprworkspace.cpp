#include "hyprworkspace.h"

#include <qobject.h>
#include <qproperty.h>

#include "hyprdefs.h"

namespace ns::hyprland {
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
} // namespace ns::hyprland
