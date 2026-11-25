#include "hypr_monitor.h"

namespace ns::wayland::hyprland {
int HyprScreenData::propCount() const {
  return static_cast<int>(ExportedProps::PROPS_MAX);
}

QVariant HyprScreenData::varAt(qsizetype idx) const {
  using enum ExportedProps;
  switch (static_cast<ExportedProps>(idx)) {
  case Id: return QVariant::fromValue(b_id.value());
  case ActiveWorkspaceId:
    return QVariant::fromValue(b_activeWorkspaceId.value());
  default: return QVariant();
  }
};
;
} // namespace ns::wayland::hyprland
