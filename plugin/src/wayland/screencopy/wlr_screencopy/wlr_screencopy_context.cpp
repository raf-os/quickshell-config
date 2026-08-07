#include "wlr_screencopy_context.h"

#include <cstdint>

#include <private/qwaylanddisplay_p.h>
#include <private/qwaylandscreen_p.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qscreen.h>
#include <qtypes.h>

#include "wayland-wlr-screencopy-unstable-v1-client-protocol.h"
#include "wlbuffer.h"
#include "wlr_screencopy.h"

namespace ns::wayland::screencopy::wlr {
Q_DECLARE_LOGGING_CATEGORY(logNSScreencopyWlr)

WlrScreencopyContext::WlrScreencopyContext(WlrScreencopyManager *manager,
                                           QScreen              *screen)
    : manager(manager),
      screen(
          dynamic_cast<QtWaylandClient::QWaylandScreen *>(screen->handle())) {
  QObject::connect(screen,
                   &QObject::destroyed,
                   this,
                   &WlrScreencopyContext::onScreenDestroyed);
}

WlrScreencopyContext::~WlrScreencopyContext() {
  if (this->object()) this->destroy();
}

void WlrScreencopyContext::onScreenDestroyed() {
  if (this->object()) this->destroy();
  emit this->stopped();
}

void WlrScreencopyContext::zwlr_screencopy_frame_v1_buffer(uint32_t format,
                                                           uint32_t width,
                                                           uint32_t height,
                                                           uint32_t /*stride*/
) {
  this->request.width  = width;
  this->request.height = height;
  this->request.shm.formats.push_back(format);
}

void WlrScreencopyContext::zwlr_screencopy_frame_v1_linux_dmabuf(
    uint32_t format,
    uint32_t width,
    uint32_t height) {
  this->request.width  = width;
  this->request.height = height;
  this->request.dmabuf.formats.push_back(format);
}

void WlrScreencopyContext::zwlr_screencopy_frame_v1_flags(uint32_t flags) {
  this->yInvert = flags & ZWLR_SCREENCOPY_FRAME_V1_FLAGS_Y_INVERT;
}

void WlrScreencopyContext::zwlr_screencopy_frame_v1_buffer_done() {
  auto *backBuffer = this->m_swapchain.createBackBuffer(this->request);

  if (!backBuffer || !backBuffer->buffer()) {
    this->destroy();
    this->captureFrame();
    return;
  }

  if (this->copiedFirstFrame) {
    this->copy_with_damage(backBuffer->buffer());
  } else {
    this->copy(backBuffer->buffer());
  }
}

void WlrScreencopyContext::zwlr_screencopy_frame_v1_ready(uint32_t /*tvSecHi*/,
                                                          uint32_t /*tvSecLo*/,
                                                          uint32_t /*tvNsec*/) {
  this->submitFrame();
}

void WlrScreencopyContext::zwlr_screencopy_frame_v1_failed() {
  qCWarning(logNSScreencopyWlr)
      << "Screencopy failure - ending recording for" << this;
  emit stopped();
}

void WlrScreencopyContext::updateTransform(bool previouslyUnset) {
  if (previouslyUnset && this->copiedFirstFrame) this->submitFrame();
}

void WlrScreencopyContext::submitFrame() {
  this->copiedFirstFrame = true;
  if (this->transform.transform == -1) return;

  auto flipTransform = this->yInvert ? buffer::WlBufferTransform::Flipped180
                                     : buffer::WlBufferTransform::Normal0;

  m_swapchain.backBuffer()->transform =
      this->transform.transform ^ flipTransform;

  this->destroy();
  m_swapchain.swapBuffers();
  emit frameCaptured();
}

WlrScreencopyContext::OutputTransformQuery::OutputTransformQuery(
    WlrScreencopyContext *context)
    : m_context(context) {}

WlrScreencopyContext::OutputTransformQuery::~OutputTransformQuery() {
  if (this->isInitialized()) this->release();
}

void WlrScreencopyContext::OutputTransformQuery::setScreen(
    QtWaylandClient::QWaylandScreen *screen) {
  // Quickshell's original implementation is this unhinged monstrosity:
  //
  // class QWaylandScreenReflector : public QtWaylandClient::QWaylandScreen {
  // public:
  //   [[nodiscard]] int globalId() const { return this->m_outputId; }
  // };
  //
  // I am not entirely sure why this was even done, though I hope the sane
  // approach below works. My guess is it was done to attempt to fetch the
  // private member variable m_outputId? But that class already has a public
  // getter for that, QtWaylandClient::QWaylandScreen::outputId()...

  if (this->isInitialized()) this->release();

  // Like... why
  // this->init(screen->display()->wl_registry(),
  //            static_cast<QWaylandScreenReflector *>(screen)->globalId(),
  //            3);

  this->init(screen->display()->wl_registry(), screen->outputId(), 3);
}

void WlrScreencopyContext::OutputTransformQuery::output_geometry(
    qint32 /*x*/,
    qint32 /*y*/,
    qint32 /*width*/,
    qint32 /*height*/,
    qint32 /*subpixel*/,
    const QString & /*make*/,
    const QString & /*model*/,
    qint32 transform) {
  auto newTransform = this->transform == -1;
  this->transform   = transform;
  m_context->updateTransform(newTransform);
}
} // namespace ns::wayland::screencopy::wlr
