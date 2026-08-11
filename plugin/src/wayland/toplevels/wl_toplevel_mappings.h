#pragma once

#include <qhash.h>
#include <qlist.h>
#include <qobject.h>
#include <qtmetamacros.h>

#include "toplevelhandle.h"
#include "wl_toplevel_handle.h"

namespace ns::wayland::toplevels {
struct WlToplevelMap {
  WLToplevelHandle               *waylandHandle = nullptr;
  wlr::toplevels::ToplevelHandle *wlrHandle     = nullptr;
};

class WlToplevelMappings : public QObject {
  Q_OBJECT

public:
  static WlToplevelMappings *instance();

private slots:
  void onWaylandHandleReady(WLToplevelHandle *handle);
  void onWaylandHandleClosed(WLToplevelHandle *handle);

private:
  explicit WlToplevelMappings();

  QHash<WLToplevelHandle *, wlr::toplevels::ToplevelHandle *> m_mapped;
  void *m_foreignHandler = nullptr;
};
} // namespace ns::wayland::toplevels
