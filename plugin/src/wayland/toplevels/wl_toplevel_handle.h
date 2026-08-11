#pragma once

#include <qobject.h>

#include "qwayland-ext-foreign-toplevel-list-v1.h"

namespace ns::wayland::toplevels {
class WLToplevelHandle : public QObject,
                         public QtWayland::ext_foreign_toplevel_handle_v1 {
  Q_OBJECT

public:
  void ext_foreign_toplevel_handle_v1_closed() override;
  void ext_foreign_toplevel_handle_v1_done() override;
  void ext_foreign_toplevel_handle_v1_title(const QString &title) override;
  void ext_foreign_toplevel_handle_v1_app_id(const QString &app_id) override;
  void
  ext_foreign_toplevel_handle_v1_identifier(const QString &identifier) override;

  [[nodiscard]] QString appId() const;
  [[nodiscard]] QString title() const;
  [[nodiscard]] QString identifier() const;

signals:
  void closed();
  void ready();

  void titleChanged();
  void appIdChanged();
  void identifierChanged();

private:
  bool m_isReady = false;

  QString m_title;
  QString m_appId;
  QString m_identifier;
};
} // namespace ns::wayland::toplevels
