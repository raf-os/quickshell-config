#pragma once

#include <qsize.h>
#include <qtclasshelpermacros.h>
#include <wayland-client-protocol.h>

#include "wlbufferrequest.h"

namespace ns::wayland::buffer {
class WlBuffer {
public:
  virtual ~WlBuffer() = default;

  Q_DISABLE_COPY_MOVE(WlBuffer)

  [[nodiscard]] virtual wl_buffer *buffer() const = 0;
  [[nodiscard]] virtual QSize      size() const   = 0;
  [[nodiscard]] virtual bool
                isCompatible(const WlBufferRequest &request) const = 0;
  [[nodiscard]] operator bool() const { return this->buffer(); }

protected:
  explicit WlBuffer() = default;
};
} // namespace ns::wayland::buffer
