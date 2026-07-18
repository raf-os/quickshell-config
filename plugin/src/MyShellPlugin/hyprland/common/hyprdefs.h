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

struct HyprWorkspaceData {
  bool    isPersistent;
  int     id;
  int     monitorId;
  QString monitorName;
  QString name;
};

struct HyprMonitorData {
  bool    disabled;
  int     id;
  int     width;
  int     height;
  int     x;
  int     y;
  QString name;
  QString model;
  QString serial;
};
} // namespace ns::hyprland::common
