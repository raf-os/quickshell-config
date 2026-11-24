#include "layer_surface.h"

#include <any>
#include <cstdint>

#include <QtWaylandClient/private/qwaylandinputdevice_p.h>
#include <QtWaylandClient/private/qwaylandscreen_p.h>
#include <QtWaylandClient/private/qwaylandshellsurface_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>
#include <qhashfunctions.h>
#include <qloggingcategory.h>
#include <qmargins.h>
#include <qobject.h>
#include <qsize.h>
#include <qtenvironmentvariables.h>
#include <qtypes.h>

#include "layer_enums.h"
#include "layer_shell_integration.h"
#include "layer_window.h"
#include "ns_layershell_debug.h"
#include "qwayland-wlr-layer-shell-unstable-v1.h"
#include "wayland-wlr-layer-shell-unstable-v1-client-protocol.h"
#include "xdg_activation.h"
#include "xdg_activation_token.h"

namespace ns::layershell {
LayerSurface::LayerSurface(WaylandLayerShellIntegration    *shell,
                           QtWaylandClient::QWaylandWindow *window)
    : QtWaylandClient::QWaylandShellSurface(window),
      QtWayland::zwlr_layer_surface_v1(), m_shell(shell),
      m_interface(LayerWindow::get(window->window())), m_window(window) {
  wl_output *output = nullptr;

  QScreen *desiredScreen = m_interface->screen();
  if (!desiredScreen) {
    desiredScreen = window->window()->screen();
  }

  auto waylandScreen =
      dynamic_cast<QtWaylandClient::QWaylandScreen *>(desiredScreen->handle());

  if (!waylandScreen) {
    qCWarning(NS_LOG_LAYERSHELL)
        << "Creating a layer shell for a placeholder screen. Positioning will "
           "not work correctly.";
  } else {
    output = waylandScreen->output();
  }

  init(shell->get_layer_surface(window->waylandSurface()->object(), output,
                                m_interface->layer(),
                                m_interface->wlrNamespace()));

  setAnchor(m_interface->anchors());
  QObject::connect(m_interface, &LayerWindow::anchorsChanged, this, [this] {
    setAnchor(m_interface->anchors());
    if (m_interface->desiredSize().isNull()) {
      setDesiredSize(m_window->windowContentGeometry().size());
    } else {
      setDesiredSize(m_interface->desiredSize());
    }
  });

  setExclusiveZone(m_interface->exclusionZone());
  QObject::connect(m_interface, &LayerWindow::exclusionZoneChanged, this,
                   [this] { setExclusiveZone(m_interface->exclusionZone()); });

  setExclusiveEdge(m_interface->exclusiveEdge());
  QObject::connect(m_interface, &LayerWindow::exclusiveEdgeChanged, this,
                   [this] { setExclusiveZone(m_interface->exclusiveEdge()); });

  setMargins(m_interface->margins());
  QObject::connect(m_interface, &LayerWindow::marginsChanged, this,
                   [this] { setMargins(m_interface->margins()); });

  QObject::connect(m_interface, &LayerWindow::desiredSizeChanged, this, [this] {
    if (!m_interface->desiredSize().isNull()) {
      setDesiredSize(m_interface->desiredSize());
    }
  });

  setKeyboardInteractivity(m_interface->keyboardInteractivity());
  QObject::connect(
      m_interface, &LayerWindow::keyboardInteractivityChanged, this, [this] {
        setKeyboardInteractivity(m_interface->keyboardInteractivity());
      });

  if (m_interface->desiredSize().isNull()) {
    setDesiredSize(window->windowContentGeometry().size());
  } else {
    setDesiredSize(m_interface->desiredSize());
  }
}

LayerSurface::~LayerSurface() { destroy(); }

bool LayerSurface::isExposed() const { return m_configured; }

void LayerSurface::attachPopup(QtWaylandClient::QWaylandShellSurface *popup) {
  std::any anyRole = popup->surfaceRole();

  try {
    auto role = std::any_cast<::xdg_popup *>(&anyRole);
    get_popup(*role);
  } catch (const std::bad_any_cast & /*unused*/) {
    qCWarning(NS_LOG_LAYERSHELL) << "Attempted to attach invalid popup.";
    return;
  }
}

void LayerSurface::zwlr_layer_surface_v1_closed() {
  window()->window()->close();
}

void LayerSurface::zwlr_layer_surface_v1_configure(uint32_t serial,
                                                   uint32_t width,
                                                   uint32_t height) {
  ack_configure(serial);
  m_pendingSize = QSize(width, height);

  if (!m_configured) {
    // First configure is the initial exposure of the window
    m_configured = true;
    applyConfigure();
    sendExpose();
  } else {
    // Subsequent configures are resizes, and are queued up
    window()->applyConfigureWhenPossible();
  }
}

void LayerSurface::sendExpose() { window()->updateExposure(); }

void LayerSurface::applyConfigure() {
  window()->resizeFromApplyConfigure(m_pendingSize);
}

void LayerSurface::setWindowSize(const QSize &size) {
  if (m_interface->desiredSize().isNull()) {
    setDesiredSize(size);
  }
}

void LayerSurface::setAnchor(uint32_t anchor) { set_anchor(anchor); }

void LayerSurface::setDesiredSize(const QSize &size) {
  const bool isHorizontallyConstrained =
      m_interface->anchors().testFlags({LayerAnchor::Left, LayerAnchor::Right});
  const bool isVerticallyConstrained =
      m_interface->anchors().testFlags({LayerAnchor::Top, LayerAnchor::Bottom});

  QSize effectiveSize = size;
  if (isHorizontallyConstrained) {
    effectiveSize.setWidth(0);
  }
  if (isVerticallyConstrained) {
    effectiveSize.setHeight(0);
  }

  set_size(effectiveSize.width(), effectiveSize.height());
}

void LayerSurface::setExclusiveZone(int32_t zone) { set_exclusive_zone(zone); }

void LayerSurface::setExclusiveEdge(uint32_t edge) {
  if (zwlr_layer_surface_v1_get_version(object()) >=
      ZWLR_LAYER_SURFACE_V1_SET_EXCLUSIVE_EDGE_SINCE_VERSION)
  {
    set_exclusive_edge(edge);
  }
}

void LayerSurface::setMargins(const QMargins &margins) {
  set_margin(margins.top(), margins.right(), margins.bottom(), margins.left());
}

void LayerSurface::setKeyboardInteractivity(uint32_t interactivity) {
  set_keyboard_interactivity(interactivity);
}

void LayerSurface::setLayer(uint32_t layer) {
  if (zwlr_layer_surface_v1_get_version(object()) >=
      ZWLR_LAYER_SURFACE_V1_SET_LAYER_SINCE_VERSION)
  {
    set_layer(layer);
  }
}

bool LayerSurface::requestActivate() {
  wayland::xdg::XdgActivation *activation = m_shell->activation();
  if (!activation->isActive()) return false;

  if (!m_activationToken.isEmpty()) {
    activation->activate(m_activationToken, window()->wlSurface());
    m_activationToken.clear();
    return true;
  } else if (const auto token = qEnvironmentVariable("XDG_ACTIVATION_TOKEN");
             !token.isEmpty())
  {
    activation->activate(token, window()->wlSurface());
    qunsetenv("XDG_ACTIVATION_TOKEN");
    return true;
  } else {
    const auto focusWindow = QGuiApplication::focusWindow();
    const auto wlWindow    = focusWindow && focusWindow->handle()
                               ? static_cast<QtWaylandClient::QWaylandWindow *>(
                                     focusWindow->handle())
                               : window();

    if (const auto seat = wlWindow->display()->lastInputDevice()) {
      const auto tokenProvider = activation->requestXdgActivationToken(
          wlWindow->display(), wlWindow->wlSurface(), seat->serial(),
          QString());
      QObject::connect(tokenProvider, &wayland::xdg::XdgActivationToken::done,
                       tokenProvider, &QObject::deleteLater);
      return true;
    }
  }

  return false;
}

bool LayerSurface::requestActivateOnShow() {
  if (m_interface->keyboardInteractivity() == KeyboardInteractivity::None) {
    return false;
  }

  if (m_window->window()->property("_q_showWithoutActivating").toBool()) {
    return false;
  }

  return requestActivate();
}

void LayerSurface::setXdgActivationToken(const QString &token) {
  m_activationToken = token;
}

void LayerSurface::requestXdgActivationToken(quint32 serial) {
  wayland::xdg::XdgActivation *activation = m_shell->activation();
  if (!activation->isActive()) return;

  auto tokenProvider = activation->requestXdgActivationToken(
      window()->display(), window()->wlSurface(), serial, QString());

  QObject::connect(tokenProvider, &wayland::xdg::XdgActivationToken::done, this,
                   [this](const QString &token) {
                     emit window() -> xdgActivationTokenCreated(token);
                   });

  QObject::connect(tokenProvider, &wayland::xdg::XdgActivationToken::done,
                   tokenProvider, &QObject::deleteLater);
}
} // namespace ns::layershell
