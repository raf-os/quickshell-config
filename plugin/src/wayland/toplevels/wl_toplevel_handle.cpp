#include "wl_toplevel_handle.h"

#include <qobject.h>

namespace ns::wayland::toplevels {
QString WLToplevelHandle::appId() const { return m_appId; }
QString WLToplevelHandle::title() const { return m_title; }
QString WLToplevelHandle::identifier() const { return m_identifier; }

void WLToplevelHandle::ext_foreign_toplevel_handle_v1_closed() {
  this->destroy();
  emit closed();
  delete this;
}

void WLToplevelHandle::ext_foreign_toplevel_handle_v1_done() {
  auto wasReady = m_isReady;
  m_isReady     = true;
  if (!wasReady) {
    emit ready();
  }
}

void WLToplevelHandle::ext_foreign_toplevel_handle_v1_title(
    const QString &title) {
  m_title = title;
  emit titleChanged();
}

void WLToplevelHandle::ext_foreign_toplevel_handle_v1_app_id(
    const QString &app_id) {
  m_appId = app_id;
  emit appIdChanged();
}

void WLToplevelHandle::ext_foreign_toplevel_handle_v1_identifier(
    const QString &identifier) {
  m_identifier = identifier;
  emit identifierChanged();
}
} // namespace ns::wayland::toplevels
