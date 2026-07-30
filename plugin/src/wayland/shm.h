#pragma once

// WARNING: shm buffers are a fallback path, and is much,
// much slower than its alternatives. This implementation is taken off of
// quickshell, and contains inneficient operations, which is even acknowledged
// by them. Looking into improving this is a very low priority. It might be even
// desirable to completely disable this path and just pretend buffer
// registration failed.

#include <cstdint>
#include <memory>

#include <private/qwaylandshmbackingstore_p.h>
#include <qdebug.h>
#include <qquickwindow.h>
#include <qsgtexture.h>
#include <qsize.h>
#include <qtclasshelpermacros.h>
#include <wayland-client-protocol.h>

#include "qsg.h"
#include "wlbuffer.h"
#include "wlbufferrequest.h"

namespace ns::wayland::buffer::shm {
class WlShmBuffer : public WlBuffer {
public:
  ~WlShmBuffer() override;
  Q_DISABLE_COPY_MOVE(WlShmBuffer)

  [[nodiscard]] wl_buffer *buffer() const override {
    return this->shmBuffer->buffer();
  }
  [[nodiscard]] QSize size() const override { return this->shmBuffer->size(); }
  [[nodiscard]] bool
  isCompatible(const WlBufferRequest &request) const override;
  [[nodiscard]] WlBufferQSGTexture *
  createQsgTexture(QQuickWindow *window) const override;

private:
  WlShmBuffer(QtWaylandClient::QWaylandShmBuffer *shmBuffer,
              uint32_t                            format)
      : shmBuffer(shmBuffer),
        format(format) {}

  std::shared_ptr<QtWaylandClient::QWaylandShmBuffer> shmBuffer;

  uint32_t format;

  friend QDebug &operator<<(QDebug            &debug,
                            const WlShmBuffer *buffer);
  friend class WlShmBufferQSGTexture;
  friend class ShmbufManager;
};

QDebug &operator<<(QDebug            &debug,
                   const WlShmBuffer *buffer);

class WlShmBufferQSGTexture : public WlBufferQSGTexture {
public:
  [[nodiscard]] QSGTexture *texture() const override {
    return this->qsgTexture.get();
  }
  void sync(const WlBuffer *buffer,
            QQuickWindow   *window) override;

private:
  WlShmBufferQSGTexture() = default;

  std::shared_ptr<QtWaylandClient::QWaylandShmBuffer> shmBuffer;
  std::unique_ptr<QSGTexture>                         qsgTexture;

  friend class WlShmBuffer;
};

class ShmbufManager {
public:
  [[nodiscard]] static WlBuffer *createShmbuf(const WlBufferRequest &request);
};
} // namespace ns::wayland::buffer::shm
