#pragma once

#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qwayland-wlr-foreign-toplevel-management-unstable-v1.h>

namespace ns::hyprland::toplevels {
class ToplevelHandle : public QObject,
                       public QtWayland::zwlr_foreign_toplevel_handle_v1 {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(QString appId READ appId NOTIFY appIdChanged)
  Q_PROPERTY(QString title READ title NOTIFY titleChanged)
  Q_PROPERTY(ns::hyprland::toplevels::ToplevelHandle *parent READ parent NOTIFY
                 parentChanged)
  Q_PROPERTY(bool activated READ activated NOTIFY activatedChanged)
  Q_PROPERTY(bool fullscreen READ fullscreen NOTIFY fullscreenChanged)

public:
  [[nodiscard]] QString         appId() const;
  [[nodiscard]] QString         title() const;
  [[nodiscard]] ToplevelHandle *parent() const;
  [[nodiscard]] bool            activated() const;
  [[nodiscard]] bool            maximized() const;
  [[nodiscard]] bool            minimized() const;
  [[nodiscard]] bool            fullscreen() const;

  Q_INVOKABLE void activate();
  Q_INVOKABLE void close();

signals:
  void ready();
  void closed();

  void appIdChanged();
  void titleChanged();
  void activatedChanged();
  void maximizedChanged();
  void minimizedChanged();
  void fullscreenChanged();
  void parentChanged();

private slots:
  void onParentClosed();

private:
  void zwlr_foreign_toplevel_handle_v1_done() override;
  void zwlr_foreign_toplevel_handle_v1_closed() override;
  void zwlr_foreign_toplevel_handle_v1_app_id(const QString &appId) override;
  void zwlr_foreign_toplevel_handle_v1_title(const QString &title) override;
  void zwlr_foreign_toplevel_handle_v1_state(wl_array *stateArray) override;
  void zwlr_foreign_toplevel_handle_v1_output_enter(wl_output *output) override;
  void zwlr_foreign_toplevel_handle_v1_output_leave(wl_output *output) override;
  void zwlr_foreign_toplevel_handle_v1_parent(
      ::zwlr_foreign_toplevel_handle_v1 *parent) override;

  bool            isReady = false;
  QString         m_appId;
  QString         m_title;
  ToplevelHandle *m_parent     = nullptr;
  bool            m_activated  = false;
  bool            m_maximized  = false;
  bool            m_minimized  = false;
  bool            m_fullscreen = false;
};
} // namespace ns::hyprland::toplevels
