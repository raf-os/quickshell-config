#pragma once

#include <qscreen.h>
#include <qtclasshelpermacros.h>
#include <qwaylandclientextension.h>

#include "qwayland-ext-image-capture-source-v1.h"
#include "qwayland-ext-image-copy-capture-v1.h"
#include "screencopy_context.h"
#include "wayland-ext-image-capture-source-v1-client-protocol.h"
#include "wl_toplevel_handle.h"

namespace ns::wayland::screencopy::icc {
class IccManager : public QWaylandClientExtensionTemplate<IccManager>,
                   public QtWayland::ext_image_copy_capture_manager_v1 {
public:
  ScreencopyContext *createSession(::ext_image_capture_source_v1 *source);

  static IccManager *instance();

private:
  explicit IccManager();
};

class IccOutputSourceManager
    : public QWaylandClientExtensionTemplate<IccOutputSourceManager>,
      public QtWayland::ext_output_image_capture_source_manager_v1 {
public:
  static IccOutputSourceManager *instance();

  ::ext_image_capture_source_v1 *createSource(QScreen *output);

private:
  explicit IccOutputSourceManager();
};

class IccForeignToplevelManager
    : public QWaylandClientExtensionTemplate<IccForeignToplevelManager>,
      public QtWayland::ext_foreign_toplevel_image_capture_source_manager_v1 {
public:
  static IccForeignToplevelManager *instance();

  ::ext_image_capture_source_v1 *
  createSource(toplevels::WLToplevelHandle *handle);

private:
  explicit IccForeignToplevelManager();
};
} // namespace ns::wayland::screencopy::icc
