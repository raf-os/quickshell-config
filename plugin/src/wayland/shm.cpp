#include "shm.h"

#include <algorithm>

#include <private/qwaylandintegration_p.h>
#include <private/qwaylandshm_p.h>
#include <private/qwaylandshmbackingstore_p.h>
#include <qdebug.h>
#include <qloggingcategory.h>
#include <qquickwindow.h>
#include <qsize.h>
#include <wayland-client-protocol.h>

#include "qsg.h"
#include "wlbuffer.h"
#include "wlbufferrequest.h"

namespace ns::wayland::buffer::shm {
Q_LOGGING_CATEGORY(logNSShm,
                   "nightshell.wayland.buffer.shm")

bool WlShmBuffer::isCompatible(const WlBufferRequest &request) const {
  if (QSize(static_cast<int>(request.width),
            static_cast<int>(request.height)) != this->size()) {
    return false;
  }

  auto matchingFormat = std::ranges::find(request.shm.formats, this->format);
  return matchingFormat != request.shm.formats.end();
}

QDebug &operator<<(QDebug            &debug,
                   const WlShmBuffer *buffer) {
  auto saver = QDebugStateSaver(debug);
  debug.nospace();

  if (buffer) {
    auto fmt = QtWaylandClient::QWaylandShm::formatFrom(
        static_cast<::wl_shm_format>(buffer->format));

    debug << "WlShmBuffer(" << static_cast<const void *>(buffer)
          << ", size=" << buffer->size() << ", format=" << fmt << ")";
  } else {
    debug << "WlShmBuffer(0x0)";
  }

  return debug;
}

WlShmBuffer::~WlShmBuffer() { qCDebug(logNSShm) << "Destroyed" << this; }

WlBufferQSGTexture *WlShmBuffer::createQsgTexture(QQuickWindow *window) const {
  auto *texture = new WlShmBufferQSGTexture();

  texture->shmBuffer = this->shmBuffer;

  texture->qsgTexture.reset(
      window->createTextureFromImage(*this->shmBuffer->image()));
  texture->sync(this, window);
  return texture;
}

void WlShmBufferQSGTexture::sync(const WlBuffer * /*unused*/,
                                 QQuickWindow *window) {
  // Note: very expensive. Sub-optimal method for sub-optimal shm buffers.
  this->qsgTexture.reset(
      window->createTextureFromImage(*this->shmBuffer->image()));
}

/**
 * May return nullptr
 */
WlBuffer *ShmbufManager::createShmbuf(const WlBufferRequest &request) {
  if (request.shm.formats.isEmpty()) return nullptr;

  static const auto *waylandIntegration =
      QtWaylandClient::QWaylandIntegration::instance();
  auto *display = waylandIntegration->display();

  auto format = request.shm.formats.at(0);
  if (format == 0) return nullptr;

  auto *buffer = new WlShmBuffer(new QtWaylandClient::QWaylandShmBuffer(
                                     display,
                                     QSize(static_cast<int>(request.width),
                                           static_cast<int>(request.height)),
                                     QtWaylandClient::QWaylandShm::formatFrom(
                                         static_cast<::wl_shm_format>(format))),
                                 format);

  qCDebug(logNSShm) << "Created shmbuf" << buffer;
  return buffer;
}
} // namespace ns::wayland::buffer::shm
