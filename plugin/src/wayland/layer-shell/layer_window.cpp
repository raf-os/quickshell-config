#include "layer_window.h"

#include <memory>

#include <private/qwaylandwindow_p.h>
#include <qcoreevent.h>
#include <qevent.h>
#include <qhashfunctions.h>
#include <qloggingcategory.h>
#include <qmap.h>
#include <qmargins.h>
#include <qobject.h>
#include <qpointer.h>
#include <qscreen.h>
#include <qsize.h>
#include <qwindow.h>

#include "layer_shell_integration.h"
#include "ns_layershell_debug.h"

namespace ns::layershell {
namespace {
static QMap<QWindow *, LayerWindow *> s_map;
}

class LayerWindowPrivate {
public:
  LayerWindowPrivate(QWindow *window) : parentWindow(window) {}

  QWindow          *parentWindow;
  QPointer<QScreen> screen;
};

LayerWindow::LayerWindow(QWindow *window)
    : QObject(window), d(std::make_unique<LayerWindowPrivate>(window)) {
  s_map.insert(d->parentWindow, this);
  window->installEventFilter(this);

  if (window->handle()) {
    initializeShell();
  }
}

LayerWindow::~LayerWindow() { s_map.remove(d->parentWindow); }

bool LayerWindow::eventFilter(QObject *watched, QEvent *event) {
  auto window = qobject_cast<QWindow *>(watched);
  if (!window) {
    return false;
  }
  if (event->type() == QEvent::PlatformSurface) {
    if (auto pse = static_cast<QPlatformSurfaceEvent *>(event);
        pse->surfaceEventType() == QPlatformSurfaceEvent::SurfaceCreated)
    {
      initializeShell();
    }
  }
  return false;
}

void LayerWindow::initializeShell() {
  auto waylandWindow = dynamic_cast<QtWaylandClient::QWaylandWindow *>(
      d->parentWindow->handle());

  if (!waylandWindow) {
    qCWarning(NS_LOG_LAYERSHELL)
        << d->parentWindow
        << "is not a wayland window. Unable to create a layer surface.";
    return;
  }

  static WaylandLayerShellIntegration *shellIntegration = nullptr;
  if (!shellIntegration) {
    shellIntegration = new WaylandLayerShellIntegration();
    if (!shellIntegration->initialize(waylandWindow->display())) {
      delete shellIntegration;
      shellIntegration = nullptr;
      qCWarning(NS_LOG_LAYERSHELL)
          << "Unable to initialize layershell integration.";
      return;
    }
  }
  waylandWindow->setShellIntegration(shellIntegration);
}

LayerWindow *LayerWindow::get(QWindow *window) {
  if (!window) return nullptr;

  auto layerShellWindow = s_map.value(window);
  if (layerShellWindow) return layerShellWindow;
  return new LayerWindow(window);
}

LayerWindow *LayerWindow::qmlAttachedProperties(QObject *object) {
  return get(qobject_cast<QWindow *>(object));
}

void LayerWindow::setScreen(QScreen *screen) {
  if (d->screen == screen) return;

  d->screen = screen;
  emit screenChanged();
}
QScreen *LayerWindow::screen() const { return d->screen.get(); }
} // namespace ns::layershell
