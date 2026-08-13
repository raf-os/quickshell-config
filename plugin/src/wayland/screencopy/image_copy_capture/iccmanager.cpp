#include "iccmanager.h"

#include <private/qwaylandscreen_p.h>
#include <qloggingcategory.h>
#include <qscreen.h>
#include <qwaylandclientextension.h>

#include "icc_screencopy_context.h"
#include "screencopy_context.h"
#include "wayland-ext-image-capture-source-v1-client-protocol.h"
#include "wl_toplevel_handle.h"

namespace ns::wayland::screencopy::icc {
Q_LOGGING_CATEGORY(logNSICCScreencopy,
                   "ns.screencopy.icc")

IccManager::IccManager() : QWaylandClientExtensionTemplate(1) {
  this->initialize();
}

IccManager *IccManager::instance() {
  static auto s_instance = new IccManager();
  return s_instance;
}

ScreencopyContext *
IccManager::createSession(::ext_image_capture_source_v1 *source) {
  auto session = this->create_session(source, 0);
  return new IccScreencopyContext(session);
}

ScreencopyContext *
IccManager::createContextFromToplevel(toplevels::WLToplevelHandle *toplevel) {
  auto inst = IccForeignToplevelManager::instance();
  if (!inst->isActive()) {
    qCWarning(logNSICCScreencopy)
        << "Unable to create context from toplevel: manager is not active.";
    return nullptr;
  }
  auto source = inst->createSource(toplevel);
  return this->createSession(source);
}

ScreencopyContext *IccManager::createContextFromScreen(QScreen *screen) {
  auto inst = IccOutputSourceManager::instance();
  if (!inst->isActive()) {
    qCWarning(logNSICCScreencopy)
        << "Unable to create context from screen: manager is not active.";
    return nullptr;
  }
  auto source = inst->createSource(screen);
  return this->createSession(source);
}

IccOutputSourceManager::IccOutputSourceManager()
    : QWaylandClientExtensionTemplate(1) {
  this->initialize();
}

IccOutputSourceManager *IccOutputSourceManager::instance() {
  static auto s_instance = new IccOutputSourceManager();
  return s_instance;
}

::ext_image_capture_source_v1 *
IccOutputSourceManager::createSource(QScreen *screen) {
  auto wlOutput =
      dynamic_cast<QtWaylandClient::QWaylandScreen *>(screen->handle());
  if (!wlOutput) return nullptr;

  return this->create_source(wlOutput->output());
}

IccForeignToplevelManager::IccForeignToplevelManager()
    : QWaylandClientExtensionTemplate(1) {
  this->initialize();
}

IccForeignToplevelManager *IccForeignToplevelManager::instance() {
  static auto s_instance = new IccForeignToplevelManager();
  return s_instance;
}

::ext_image_capture_source_v1 *
IccForeignToplevelManager::createSource(toplevels::WLToplevelHandle *handle) {
  if (handle == nullptr) return nullptr;

  return this->create_source(handle->object());
}
} // namespace ns::wayland::screencopy::icc
