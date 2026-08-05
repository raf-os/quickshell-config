#pragma once

#include <memory>

#include <qquickwindow.h>
#include <qsize.h>
#include <qtclasshelpermacros.h>
#include <wayland-client-protocol.h>

#include "wlbufferrequest.h"

namespace ns::wayland::buffer {
class WlBufferQSGTexture;

struct WlBufferTransform {
  enum Transform : uint8_t {
    Normal0    = 0,
    Normal90   = 1,
    Normal180  = 2,
    Normal270  = 3,
    Flipped0   = 4,
    Flipped90  = 5,
    Flipped180 = 6,
    Flipped270 = 7,
  } transform = Normal0;

  WlBufferTransform() = default;
  WlBufferTransform(uint8_t transform)
      : transform(static_cast<Transform>(transform)) {}

  // I get the general idea, but my brain too smooth for the details

  // Clearing bit 3 should be similar to doing (degree % 360) so it loops around
  // to zero on a full rotation, but why are all the bits left of that 1? Not
  // that it would matter, the highest number is 7 (=0b111) so anything past
  // that should not matter
  [[nodiscard]] int degrees() const {
    return 90 * (this->transform & 0b11111011);
  }
  [[nodiscard]] bool flip() const { return this->transform & 0b00000100; }
  [[nodiscard]] bool flipSize() const { return this->transform & 0b00000001; }

  void apply(QMatrix4x4 &matrix) const {
    // now this here makes more sense for a troglodyte who sucks at linear
    // algebra like me

    // rotate around y axis = horizontal flip
    matrix.rotate(this->flip() ? 180 : 0, 0, 1, 0);
    // rotating around z axis = the "usual" 2d rotation, in increments of 90
    // degrees
    matrix.rotate(static_cast<float>(this->degrees()), 0, 0, 1);
  }
};

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

  WlBufferTransform transform;

protected:
  explicit WlBuffer() = default;
};

class WlBufferSwapchain {
public:
  [[nodiscard]] WlBuffer *createBackBuffer(const WlBufferRequest &request,
                                           bool *newBuffer = nullptr);

  void swapBuffers() { this->presentSecondBuffer = !this->presentSecondBuffer; }
  bool shouldPresentSecondBuffer() const { return this->presentSecondBuffer; }

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
