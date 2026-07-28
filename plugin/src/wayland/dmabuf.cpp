#include "dmabuf.h"

#include <algorithm>

#include <gbm.h>
#include <qdebug.h>
#include <qloggingcategory.h>
#include <unistd.h>
#include <wayland-client-protocol.h>

#include "wlbufferrequest.h"

namespace ns::wayland::buffer::dmabuf {
Q_LOGGING_CATEGORY(logNSDmabuf,
                   "nightshell.wayland.buffer.dmabuf")

namespace {}

QDebug &operator<<(QDebug          &debug,
                   const FourCCStr &fourcc) {
  debug << fourcc.cStr();
  return debug;
}
QDebug &operator<<(QDebug             &debug,
                   const FourCCModStr &fourcc) {
  debug << fourcc.cStr();
  return debug;
}

WlDmaBuffer::~WlDmaBuffer() {
  if (this->m_buffer) {
    wl_buffer_destroy(this->m_buffer);
  }

  if (this->bo) {
    gbm_bo_destroy(this->bo);
  }

  for (auto i = 0; i < this->planeCount; ++i) {
    const auto &plane = this->planes[i];
    if (plane.fd) close(plane.fd);
  }

  delete[] this->planes;
}

WlDmaBuffer::WlDmaBuffer(WlDmaBuffer &&other) noexcept
    : device(std::move(other.device)),
      bo(other.bo),
      m_buffer(other.m_buffer),
      planes(other.planes) {
  other.m_buffer   = nullptr;
  other.bo         = nullptr;
  other.planeCount = 0;
}

WlDmaBuffer &WlDmaBuffer::operator=(WlDmaBuffer &&other) noexcept {
  this->~WlDmaBuffer();
  new (this) WlDmaBuffer(std::move(other));
  return *this;
}

bool WlDmaBuffer::isCompatible(const WlBufferRequest &request) const {
  if (request.width != this->width || request.height != this->height)
    return false;

  auto matchingFormat =
      std::ranges::find_if(request.dmabuf.formats, [this](const auto &format) {
        if (format.format != this->format) return false;
        return format.format == this->format &&
               (format.modifiers.isEmpty() ||
                (format.implicit && this->usedImplicitModifier) ||
                std::ranges::find(format.modifiers, this->modifier) !=
                    format.modifiers.end());
      });

  return matchingFormat != request.dmabuf.formats.end();
}
} // namespace ns::wayland::buffer::dmabuf
