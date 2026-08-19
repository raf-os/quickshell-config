#include "input_inhibit_manager.h"

#include <private/qwaylanddisplay_p.h>
#include <private/qwaylandinputdevice_p.h>
#include <private/qwaylandscreen_p.h>
#include <private/qwaylandsurface_p.h>
#include <private/qwaylandwindow_p.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qquickwindow.h>
#include <qwaylandclientextension.h>

#include "shortcut_inhibitor.h"

namespace ns::wayland::inputinhibit {
Q_LOGGING_CATEGORY(logNSInputInhibit,
                   "ns.wayland.input_inhibit")

WlInputInhibitManager::WlInputInhibitManager()
    : QWaylandClientExtensionTemplate(1) {
  this->initialize();
}

WlInputInhibitManager::~WlInputInhibitManager() {
  if (this->isActive()) {
    this->destroy();
  }
}

WlInputInhibitManager *WlInputInhibitManager::instance() {
  static auto s_instance = new WlInputInhibitManager();
  return s_instance;
}

void WlInputInhibitManager::initWindow(QQuickWindow *window) {
  if (!window) return;
  if (!m_inputDevice.isNull()) return;

  auto qwlwin =
      dynamic_cast<QtWaylandClient::QWaylandWindow *>(window->handle());
  QtWaylandClient::QWaylandDisplay *qwldisp = qwlwin->display();
  m_inputDevice                             = qwldisp->currentInputDevice();

  emit this->ready();
}

bool WlInputInhibitManager::inhibitForWindow(
    QQuickWindow        *window,
    WlShortcutInhibitor *outInhibitor) {
  if (!window) {
    qCDebug(logNSInputInhibit) << "WlInputInhibitManager::inhibitForWindow: "
                                  "Called without a window object.";
    return false;
  }

  this->initWindow(window);

  auto it = m_activeInhibitors.constFind(window);
  if (it != m_activeInhibitors.constEnd()) {
    qCWarning(logNSInputInhibit)
        << "Attempted to inhibit shortcuts for window" << window
        << " but it's already active for said window.";
    return false;
  }

  auto qwlwin =
      static_cast<QtWaylandClient::QWaylandWindow *>(window->handle());
  auto surface = qwlwin->waylandSurface();
  auto seat    = m_inputDevice->wl_seat();

  auto inhibitor =
      new WlShortcutInhibitor(this->inhibit_shortcuts(surface->object(), seat));

  m_activeInhibitors.insert(window, inhibitor);

  QObject::connect(window, &QObject::destroyed, this, [this, window] {
    this->onWindowDestroyed(window);
  });

  inhibitor->setParent(window);

  emit windowInhibited(window);
  if (outInhibitor) {
    outInhibitor = inhibitor;
  }
  return true;
}

bool WlInputInhibitManager::stopInhibitingWindow(QQuickWindow *window) {
  auto it = m_activeInhibitors.find(window);

  if (it == m_activeInhibitors.end()) {
    qCDebug(logNSInputInhibit)
        << "Attempted to stop inhibiting sortcuts for" << window
        << ", but said window is not actively inhibiting.";
    return false;
  }

  auto inhibitor = it.value();
  if (inhibitor) {
    inhibitor->stopInhibiting();
  }

  return true;
}

void WlInputInhibitManager::onWindowDestroyed(QQuickWindow *window) {
  m_activeInhibitors.remove(window);
}
} // namespace ns::wayland::inputinhibit
