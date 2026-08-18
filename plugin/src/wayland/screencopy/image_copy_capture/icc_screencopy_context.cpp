#include "icc_screencopy_context.h"

#include <cstddef>
#include <cstdint>

#include <QtCore>
#include <qloggingcategory.h>
#include <qsggeometry.h>

#include "qwayland-ext-image-copy-capture-v1.h"
#include "wayland-ext-image-copy-capture-v1-client-protocol.h"
#include "wlbufferrequest.h"

namespace ns::wayland::screencopy::icc {
Q_DECLARE_LOGGING_CATEGORY(logNSICCScreencopy) // from iccmanager.cpp

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

void IccScreencopyContext::captureFrame() {
  if (this->CCAPTURE_FRAME::object() || m_capturePending) return;

  if (m_statePending) m_capturePending = true;
  else this->commitCapture();
}

// ext-image-copy-capture-session-v1
void IccScreencopyContext::ext_image_copy_capture_session_v1_buffer_size(
    uint32_t width,
    uint32_t height) {
  this->clearOldState();

  m_request.width  = width;
  m_request.height = height;
}

void IccScreencopyContext::ext_image_copy_capture_session_v1_shm_format(
    uint32_t format) {
  this->clearOldState();

  m_request.shm.formats.push_back(format);
}

void IccScreencopyContext::ext_image_copy_capture_session_v1_dmabuf_device(
    wl_array *device) {
  this->clearOldState();

  if (device->size != sizeof(dev_t)) {
    qCFatal(logNSICCScreencopy)
        << "Size of dev_t does not match the compositor's. Try "
           "recompiling.\n\tCOMPOSITOR:"
        << device->size << "bits\n\tNIGHTSHELL:" << sizeof(dev_t) << "bits";
  }

  m_request.dmabuf.device = *reinterpret_cast<dev_t *>(device->data);
}

void IccScreencopyContext::ext_image_copy_capture_session_v1_dmabuf_format(
    uint32_t  format,
    wl_array *modifiers) {
  this->clearOldState();

  auto *modifierArray = reinterpret_cast<uint64_t *>(modifiers->data);
  auto  modCount      = modifiers->size / sizeof(uint64_t);

  auto reqFormat = buffer::WlBufferRequest::DmaFormat(format);

  for (size_t i = 0; i != modCount; i++) {
    reqFormat.pushMod(modifierArray[i]);
  }

  m_request.dmabuf.formats.push_back(reqFormat);
}

void IccScreencopyContext::ext_image_copy_capture_session_v1_done() {
  m_statePending = false;

  if (m_capturePending) {
    this->commitCapture();
  }
}

void IccScreencopyContext::ext_image_copy_capture_session_v1_stopped() {
  emit stopped();
}

void IccScreencopyContext::clearOldState() {
  if (!m_statePending) {
    m_request      = buffer::WlBufferRequest();
    m_statePending = true;
  }
}

void IccScreencopyContext::commitCapture() {
  m_capturePending = false;

  auto  newBuffer  = false;
  auto *backBuffer = m_swapchain.createBackBuffer(m_request, &newBuffer);

  if (!backBuffer || !backBuffer->buffer()) {
    qCWarning(logNSICCScreencopy) << "Failed creating backbuffer. Waiting for "
                                     "new parameters before trying again.";
    return;
  }

  CCAPTURE_FRAME::init(this->CCAPTURE_SESSION::create_frame());
  CCAPTURE_FRAME::attach_buffer(backBuffer->buffer());

  if (newBuffer) {
    // This buffer is empty, repaint the whole thing
    CCAPTURE_FRAME::damage_buffer(0,
                                  0,
                                  static_cast<int>(m_request.width),
                                  static_cast<int>(m_request.height));

    m_lastDamage = QRect();
  } else if (!m_lastDamage.isEmpty()) {
    CCAPTURE_FRAME::damage_buffer(m_lastDamage.x(),
                                  m_lastDamage.y(),
                                  m_lastDamage.width(),
                                  m_lastDamage.height());

    // Damage was already applied, clear it
    m_lastDamage = QRect();
  }

  CCAPTURE_FRAME::capture();
}

// ext-image-copy-capture-frame-v1
void IccScreencopyContext::ext_image_copy_capture_frame_v1_transform(
    uint32_t transform) {
  m_swapchain.backBuffer()->transform = transform;
}

void IccScreencopyContext::ext_image_copy_capture_frame_v1_damage(
    int32_t x,
    int32_t y,
    int32_t width,
    int32_t height) {
  m_damage = m_damage.united(QRect(x, y, width, height));
}

void IccScreencopyContext::ext_image_copy_capture_frame_v1_ready() {
  CCAPTURE_FRAME::destroy(); // Immediately destroy, as the protocol dictates

  // Present our newly created buffer
  m_swapchain.swapBuffers();
  m_lastDamage = m_damage;
  m_damage     = QRect();

  emit frameCaptured();
}

void IccScreencopyContext::ext_image_copy_capture_frame_v1_failed(
    uint32_t reason) {
  switch (static_cast<CCAPTURE_FRAME::failure_reason>(reason)) {
  case CCAPTURE_FRAME::failure_reason_buffer_constraints:
    qCWarning(logNSICCScreencopy)
        << "Frame copy failed: buffer constraints mismatched!";
    break;
  case CCAPTURE_FRAME::failure_reason_stopped:
    // Already handled
    break;
  case CCAPTURE_FRAME::failure_reason_unknown:
    qCFatal(logNSICCScreencopy)
        << "Frame copy failed: unknown error. Stopping recording.";
    emit stopped();
    break;
  }
}
} // namespace ns::wayland::screencopy::icc
