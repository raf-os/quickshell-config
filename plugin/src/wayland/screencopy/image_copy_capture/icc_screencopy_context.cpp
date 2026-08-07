#include "icc_screencopy_context.h"

#include "qwayland-ext-image-copy-capture-v1.h"
#include "wayland-ext-image-copy-capture-v1-client-protocol.h"

namespace ns::wayland::screencopy::icc {
using CCAPTURE_SESSION = QtWayland::ext_image_copy_capture_session_v1;
using CCAPTURE_FRAME   = QtWayland::ext_image_copy_capture_frame_v1;

IccScreencopyContext::IccScreencopyContext(
    ::ext_image_copy_capture_session_v1 *session)
    : CCAPTURE_SESSION(session) {}

IccScreencopyContext::~IccScreencopyContext() {
  if (this->CCAPTURE_SESSION::object()) {
    this->CCAPTURE_SESSION::destroy();
  }

  if (this->CCAPTURE_FRAME::object()) {
    this->CCAPTURE_FRAME::destroy();
  }
}

// ext-image-copy-capture-session-v1
void IccScreencopyContext::ext_image_copy_capture_session_v1_buffer_size(
    uint32_t width,
    uint32_t height) {
  return;
}

void IccScreencopyContext::ext_image_copy_capture_session_v1_shm_format(
    uint32_t format) {
  return;
}

void IccScreencopyContext::ext_image_copy_capture_session_v1_dmabuf_device(
    wl_array *device) {
  return;
}

void IccScreencopyContext::ext_image_copy_capture_session_v1_dmabuf_format(
    uint32_t  format,
    wl_array *modifiers) {
  return;
}

void IccScreencopyContext::ext_image_copy_capture_session_v1_done() { return; }

void IccScreencopyContext::ext_image_copy_capture_session_v1_stopped() {
  return;
}

// ext-image-copy-capture-frame-v1
void IccScreencopyContext::ext_image_copy_capture_frame_v1_transform(
    uint32_t transform) {
  return;
}

void IccScreencopyContext::ext_image_copy_capture_frame_v1_damage(
    int32_t x,
    int32_t y,
    int32_t width,
    int32_t height) {
  return;
}

void IccScreencopyContext::ext_image_copy_capture_frame_v1_ready() { return; }

void IccScreencopyContext::ext_image_copy_capture_frame_v1_failed(
    uint32_t reason) {
  return;
}
} // namespace ns::wayland::screencopy::icc
