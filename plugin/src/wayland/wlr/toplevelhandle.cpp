#include "toplevelhandle.h"

#include <cstddef>

#include <private/qwaylanddisplay_p.h>
#include <private/qwaylandinputdevice_p.h>
#include <private/qwaylandintegration_p.h>
#include <private/qwaylandwindow_p.h>
#include <qobject.h>
#include <qwaylandclientextension.h>
#include <wayland-util.h>

#include "qwayland-wlr-foreign-toplevel-management-unstable-v1.h"
#include "toplevelmanager.h"
#include "wayland-wlr-foreign-toplevel-management-unstable-v1-client-protocol.h"

namespace ns::wayland::wlr::toplevels {
QString         ToplevelHandle::appId() const { return m_appId; }
QString         ToplevelHandle::title() const { return m_title; }
ToplevelHandle *ToplevelHandle::parent() const { return m_parent; }
bool            ToplevelHandle::activated() const { return m_activated; }
bool            ToplevelHandle::maximized() const { return m_maximized; }
bool            ToplevelHandle::minimized() const { return m_minimized; }
bool            ToplevelHandle::fullscreen() const { return m_fullscreen; }

void ToplevelHandle::activate() {
  auto *display = QtWaylandClient::QWaylandIntegration::instance()->display();
  auto *inputDevice = display->lastInputDevice();
  if (inputDevice == nullptr) return;
  this->QtWayland::zwlr_foreign_toplevel_handle_v1::activate(
      inputDevice->object());
}

void ToplevelHandle::close() {
  this->QtWayland::zwlr_foreign_toplevel_handle_v1::close();
}

void ToplevelHandle::zwlr_foreign_toplevel_handle_v1_done() {
  auto wasReady = this->isReady;
  this->isReady = true;
  if (!wasReady) {
    emit ready();
  }
}

void ToplevelHandle::zwlr_foreign_toplevel_handle_v1_closed() {
  this->destroy();
  emit closed();
  delete this;
}

void ToplevelHandle::zwlr_foreign_toplevel_handle_v1_app_id(
    const QString &appId) {
  m_appId = appId;
  emit appIdChanged();
}

void ToplevelHandle::zwlr_foreign_toplevel_handle_v1_title(
    const QString &title) {
  m_title = title;
  emit titleChanged();
}

void ToplevelHandle::zwlr_foreign_toplevel_handle_v1_state(
    wl_array *stateArray) {
  bool activated  = false;
  bool maximized  = false;
  bool minimized  = false;
  bool fullscreen = false;

  auto *state =
      static_cast<::zwlr_foreign_toplevel_handle_v1_state *>(stateArray->data);
  auto size =
      stateArray->size / sizeof(::zwlr_foreign_toplevel_handle_v1_state);
  for (size_t i = 0; i < size; i++) {
    auto flag = state[i];
    switch (flag) {
      // clang-format off
      case ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_ACTIVATED: activated = true; break;
      case ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_MAXIMIZED: maximized = true; break;
      case ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_MINIMIZED: minimized = true; break;
      case ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_FULLSCREEN: fullscreen = true; break;
      // clang-format on
    }
  }

  if (activated != m_activated) {
    m_activated = activated;
    emit activatedChanged();
  }
  if (maximized != m_maximized) {
    m_maximized = maximized;
    emit maximizedChanged();
  }
  if (minimized != m_minimized) {
    m_minimized = minimized;
    emit minimizedChanged();
  }
  if (fullscreen != m_fullscreen) {
    m_fullscreen = fullscreen;
    emit fullscreenChanged();
  }
}

void ToplevelHandle::zwlr_foreign_toplevel_handle_v1_output_enter(
    wl_output *output) {
  // TODO
}

void ToplevelHandle::zwlr_foreign_toplevel_handle_v1_output_leave(
    wl_output *output) {
  // TODO
}

void ToplevelHandle::zwlr_foreign_toplevel_handle_v1_parent(
    ::zwlr_foreign_toplevel_handle_v1 *parent) {
  auto *handle = ToplevelManager::instance()->handleFor(parent);

  if (handle != m_parent) {
    if (m_parent != nullptr) {
      QObject::disconnect(m_parent, nullptr, this, nullptr);
    }

    m_parent = handle;

    if (handle != nullptr) {
      QObject::connect(handle,
                       &ToplevelHandle::closed,
                       this,
                       &ToplevelHandle::onParentClosed);
    }

    emit parentChanged();
  }
}

void ToplevelHandle::onParentClosed() {
  m_parent = nullptr;
  emit parentChanged();
}
} // namespace ns::wayland::wlr::toplevels
