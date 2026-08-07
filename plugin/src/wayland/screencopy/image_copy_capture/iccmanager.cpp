#include "iccmanager.h"

#include <qwaylandclientextension.h>

#include "screencopy_context.h"
#include "wayland-ext-image-capture-source-v1-client-protocol.h"

namespace ns::wayland::screencopy::icc {
IccManager::IccManager() : QWaylandClientExtensionTemplate(1) {
  this->initialize();
}

IccManager *IccManager::instance() {
  static auto s_instance = new IccManager();
  return s_instance;
}

ScreencopyContext *
IccManager::createSession(::ext_image_capture_source_v1 *source) {
  return nullptr;
}

IccOutputSourceManager::IccOutputSourceManager()
    : QWaylandClientExtensionTemplate(1) {
  this->initialize();
}

IccOutputSourceManager *IccOutputSourceManager::instance() {
  static auto s_instance = new IccOutputSourceManager();
  return s_instance;
}

IccForeignToplevelManager::IccForeignToplevelManager()
    : QWaylandClientExtensionTemplate(1) {
  this->initialize();
}

IccForeignToplevelManager *IccForeignToplevelManager::instance() {
  static auto s_instance = new IccForeignToplevelManager();
  return s_instance;
}
} // namespace ns::wayland::screencopy::icc
