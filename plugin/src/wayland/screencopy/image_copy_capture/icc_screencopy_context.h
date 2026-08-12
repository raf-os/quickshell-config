#pragma once

#include <cstdint>

#include <QtCore>
#include <qsggeometry.h>
#include <qtclasshelpermacros.h>
#include <wayland-util.h>

#include "qwayland-ext-image-copy-capture-v1.h"
#include "screencopy_context.h"
#include "wayland-ext-image-copy-capture-v1-client-protocol.h"
#include "wlbufferrequest.h"

namespace ns::wayland::screencopy::icc {
class IccScreencopyContext
    : public ScreencopyContext,
      public QtWayland::ext_image_copy_capture_session_v1,
      public QtWayland::ext_image_copy_capture_frame_v1 {
public:
  IccScreencopyContext(::ext_image_copy_capture_session_v1 *session);
  ~IccScreencopyContext() override;
  Q_DISABLE_COPY_MOVE(IccScreencopyContext)

  void captureFrame() override;

protected:
  // ext-image-copy-capture-session-v1
  void ext_image_copy_capture_session_v1_buffer_size(uint32_t width,
                                                     uint32_t height) override;
  void ext_image_copy_capture_session_v1_shm_format(uint32_t format) override;
  void
  ext_image_copy_capture_session_v1_dmabuf_device(wl_array *device) override;
  void
  ext_image_copy_capture_session_v1_dmabuf_format(uint32_t  format,
                                                  wl_array *modifiers) override;
  void ext_image_copy_capture_session_v1_done() override;
  void ext_image_copy_capture_session_v1_stopped() override;

  // ext-image-copy-capture-frame-v1
  void ext_image_copy_capture_frame_v1_transform(uint32_t transform) override;
  void ext_image_copy_capture_frame_v1_damage(int32_t x,
                                              int32_t y,
                                              int32_t width,
                                              int32_t height) override;
  void ext_image_copy_capture_frame_v1_ready() override;
  void ext_image_copy_capture_frame_v1_failed(uint32_t reason) override;

private:
  void commitCapture();
  void clearOldState();

  buffer::WlBufferRequest m_request;

  bool m_capturePending = false;
  bool m_statePending   = false;

  QRect m_damage;
  QRect m_lastDamage;
};
} // namespace ns::wayland::screencopy::icc
