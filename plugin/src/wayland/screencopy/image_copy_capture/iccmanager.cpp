#include "iccmanager.h"

#include <memory>

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

IccCaptureSource::IccCaptureSource(::ext_image_capture_source_v1 *source) {
  this->init(source);
}

IccCaptureSource::~IccCaptureSource() {
  if (this->object()) this->destroy();
}

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

IccOutputSourceManager::IccOutputSourceManager()
    : QWaylandClientExtensionTemplate(1) {
  this->initialize();
}

IccOutputSourceManager *IccOutputSourceManager::instance() {
  static auto s_instance = new IccOutputSourceManager();
  return s_instance;
}

IccCaptureSource *IccOutputSourceManager::createSource(QScreen *output) {
  auto wlOutput =
      dynamic_cast<QtWaylandClient::QWaylandScreen *>(output->handle());
  if (!wlOutput) return nullptr;

  return new IccCaptureSource(this->create_source(wlOutput->output()));
}

IccForeignToplevelManager::IccForeignToplevelManager()
    : QWaylandClientExtensionTemplate(1) {
  this->initialize();
}

IccForeignToplevelManager *IccForeignToplevelManager::instance() {
  static auto s_instance = new IccForeignToplevelManager();
  return s_instance;
}

std::shared_ptr<IccCaptureSource>
IccForeignToplevelManager::createSource(toplevels::WLToplevelHandle *handle) {
  if (handle == nullptr) return nullptr;

  // return new IccCaptureSource(this->create_source(handle->object()));
  return std::make_shared<IccCaptureSource>(
      this->create_source(handle->object()));
}
} // namespace ns::wayland::screencopy::icc
