#pragma once

#include <qobject.h>
#include <qtypes.h>

namespace ns::hyprland::common {
struct HyprWindowData {
  bool    mapped;
  bool    hidden;
  QString className;
  quint32 workspaceId;
  quint32 pid;
  quint64 address;
};
} // namespace ns::hyprland::common
