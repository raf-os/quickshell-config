#pragma once

#include <qhash.h>
#include <qjsengine.h>
#include <qobject.h>
#include <qqmlengine.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qwayland-hyprland-toplevel-mapping-v1.h>
#include <qwaylandclientextension.h>

#include "hyprtoplevelhandle.h"
#include "toplevelhandle.h"

namespace ns::hyprland::toplevels {
class HyprlandToplevelMappingManager
    : public QWaylandClientExtensionTemplate<HyprlandToplevelMappingManager>,
      public QtWayland::hyprland_toplevel_mapping_manager_v1 {
  Q_OBJECT

public:
  static HyprlandToplevelMappingManager *instance();
  static HyprlandToplevelMappingManager *create(QQmlEngine *qmlEngine,
                                                QJSEngine * /* unused */);

  [[nodiscard]] bool
  hasAddress(wayland::wlr::toplevels::ToplevelHandle *handle) const;
  [[nodiscard]] quint64
  getToplevelAddress(wayland::wlr::toplevels::ToplevelHandle *handle) const;
  [[nodiscard]] wayland::wlr::toplevels::ToplevelHandle *
  getHandleForAddress(const quint64 &address) const;

signals:
  void toplevelAddressed(wayland::wlr::toplevels::ToplevelHandle *handle,
                         quint64                                  address);

private slots:
  void onToplevelReady(wayland::wlr::toplevels::ToplevelHandle *handle);
  void onToplevelDestroyed(QObject *object);

protected:
  explicit HyprlandToplevelMappingManager();

private:
  QHash<wayland::wlr::toplevels::ToplevelHandle *, quint64> m_addresses;

  void assignAddress(wayland::wlr::toplevels::ToplevelHandle *handle,
                     quint64                                  address);

  friend class HyprlandToplevelMappingHandle;
};
} // namespace ns::hyprland::toplevels
