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

    b_name         = data.name;
    b_isPersistent = data.isPersistent;
  }
}
} // namespace ns::hyprland
