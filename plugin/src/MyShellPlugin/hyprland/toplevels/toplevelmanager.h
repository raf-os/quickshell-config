#pragma once

#include <qjsengine.h>
#include <qlist.h>
#include <qqmlengine.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qwayland-wlr-foreign-toplevel-management-unstable-v1.h>
#include <qwaylandclientextension.h>

#include "toplevelhandle.h"
#include "wayland-wlr-foreign-toplevel-management-unstable-v1-client-protocol.h"

namespace ns::hyprland::toplevels {
class ToplevelManager : public QWaylandClientExtensionTemplate<ToplevelManager>,
                        public QtWayland::zwlr_foreign_toplevel_manager_v1 {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  Q_PROPERTY(
      QList<ToplevelHandle *> toplevels READ toplevels NOTIFY toplevelReady)

public:
  static ToplevelManager *instance();
  static ToplevelManager *create(QQmlEngine *qmlEngine,
                                 QJSEngine * /* unused */);

  [[nodiscard]] bool available() const;
  [[nodiscard]] ToplevelHandle *
  handleFor(::zwlr_foreign_toplevel_handle_v1 *toplevel);
  [[nodiscard]] QList<ToplevelHandle *> toplevels();

signals:
  void toplevelReady(ToplevelHandle *toplevel);

protected:
  explicit ToplevelManager();

  void zwlr_foreign_toplevel_manager_v1_toplevel(
      ::zwlr_foreign_toplevel_handle_v1 *toplevel) override;

private slots:
  void onToplevelReady();
  void onToplevelClosed();

private:
  QList<ToplevelHandle *> m_toplevels;
  QList<ToplevelHandle *> m_readyToplevels;
};
} // namespace ns::hyprland::toplevels
