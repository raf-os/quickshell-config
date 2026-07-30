#pragma once

#include <memory>

#include <qquickwindow.h>
#include <qsize.h>
#include <qtclasshelpermacros.h>
#include <wayland-client-protocol.h>

#include "wlbufferrequest.h"

namespace ns::wayland::buffer {
class WlBufferQSGTexture;

class WlBuffer {
public:
  virtual ~WlBuffer() = default;

  Q_DISABLE_COPY_MOVE(WlBuffer)

  [[nodiscard]] virtual wl_buffer *buffer() const = 0;
  [[nodiscard]] virtual QSize      size() const   = 0;
  [[nodiscard]] virtual bool
                isCompatible(const WlBufferRequest &request) const = 0;
  [[nodiscard]] operator bool() const { return this->buffer(); }

  [[nodiscard]] virtual WlBufferQSGTexture *
  createQsgTexture(QQuickWindow *window) const = 0;

protected:
  explicit WlBuffer() = default;
};

class WlBufferSwapchain {
public:
  [[nodiscard]] WlBuffer *createBackBuffer(const WlBufferRequest &request,
                                           bool *newBuffer = nullptr);

  void swapBuffers() { this->presentSecondBuffer = !this->presentSecondBuffer; }

  [[nodiscard]] WlBuffer *backBuffer() const {
    return this->presentSecondBuffer ? this->buffer1.get()
                                     : this->buffer2.get();
  }

  [[nodiscard]] WlBuffer *frontBuffer() const {
    return this->presentSecondBuffer ? this->buffer2.get()
                                     : this->buffer1.get();
  }

private:
  std::unique_ptr<WlBuffer> buffer1;
  std::unique_ptr<WlBuffer> buffer2;
  bool                      presentSecondBuffer = false;
};
} // namespace ns::wayland::buffer
