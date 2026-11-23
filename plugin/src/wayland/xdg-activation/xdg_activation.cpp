#include "xdg_activation.h"

#include <QtWaylandClient/private/qwaylanddisplay_p.h>
#include <QtWaylandClient/private/qwaylandinputdevice_p.h>

#include "xdg_activation_token.h"

namespace ns::wayland::xdg {
XdgActivation::XdgActivation() : QWaylandClientExtensionTemplate(1) {
  initialize();
}

XdgActivation::~XdgActivation() {
  if (isActive()) destroy();
}

XdgActivationToken *XdgActivation::requestXdgActivationToken(
    QtWaylandClient::QWaylandDisplay *display, struct ::wl_surface *surface,
    std::optional<uint32_t> serial, const QString &app_id) {
  auto wl       = get_activation_token();
  auto provider = new XdgActivationToken();
  provider->init(wl);

  if (surface) {
    provider->set_surface(surface);
  }
  if (!app_id.isEmpty()) {
    provider->set_app_id(app_id);
  }
  if (serial && display->lastInputDevice()) {
    provider->set_serial(serial.value(), display->lastInputDevice()->wl_seat());
  }
  provider->commit();
  return provider;
}
} // namespace ns::wayland::xdg
