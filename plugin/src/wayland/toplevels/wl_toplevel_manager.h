#pragma once

#include <qjsengine.h>
#include <qlist.h>
#include <qobject.h>
#include <qqmlengine.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qwaylandclientextension.h>

#include "qwayland-ext-foreign-toplevel-list-v1.h"
#include "wayland-ext-foreign-toplevel-list-v1-client-protocol.h"
#include "wl_toplevel_handle.h"

namespace ns::wayland::toplevels {
class WLToplevelManager
    : public QWaylandClientExtensionTemplate<WLToplevelManager>,
      public QtWayland::ext_foreign_toplevel_list_v1 {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

public:
  static WLToplevelManager *instance();
  static WLToplevelManager *create(QQmlEngine *qmlEngine,
                                   QJSEngine * /*unused*/);

  void ext_foreign_toplevel_list_v1_toplevel(
      ::ext_foreign_toplevel_handle_v1 *toplevel) override;

  [[nodiscard]] bool                      available();
  [[nodiscard]] QList<WLToplevelHandle *> toplevels();
  [[nodiscard]] QList<WLToplevelHandle *> allToplevels();

signals:
  void toplevelsChanged();
  void toplevelReady(WLToplevelHandle *toplevel);
  void toplevelClosed(WLToplevelHandle *toplevel);

private slots:
  void onToplevelClosed();
  void onToplevelReady();

private:
  explicit WLToplevelManager();

  QList<WLToplevelHandle *> m_toplevels;
  QList<WLToplevelHandle *> m_readyToplevels;
};
} // namespace ns::wayland::toplevels
