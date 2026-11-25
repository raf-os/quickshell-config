#include "hypr_workspace.h"

namespace ns::wayland::hyprland {
int HyprWorkspaceData::propCount() const {
  return static_cast<int>(ExportedProps::PROPS_MAX);
}

QVariant HyprWorkspaceData::varAt(qsizetype idx) const {
  using enum ExportedProps;
  switch (static_cast<ExportedProps>(idx)) {
  case Id:        return QVariant::fromValue(b_id.value());
  case MonitorId: return QVariant::fromValue(b_monitorId.value());
  case Address:   return QVariant::fromValue(b_address.value());
  case Name:      return QVariant::fromValue(b_name.value());
  case LastWindowAddress:
    return QVariant::fromValue(b_lastWindowAddress.value());
  default: return QVariant();
  }
}

QBindable<int> HyprWorkspaceData::bindableId() const { return &b_id; }
QBindable<int> HyprWorkspaceData::bindableMonitorId() const {
  return &b_monitorId;
}
QBindable<QString> HyprWorkspaceData::bindableAddress() const {
  return &b_address;
}
QBindable<QString> HyprWorkspaceData::bindableName() const { return &b_name; }
QBindable<quint64> HyprWorkspaceData::bindableLastWindowAddress() const {
  return &b_lastWindowAddress;
}
} // namespace ns::wayland::hyprland
