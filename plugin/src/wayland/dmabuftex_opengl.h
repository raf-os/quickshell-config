#pragma once

// this is the opengl implementation

#include <EGL/egl.h>
#include <GL/gl.h>
#include <qsgtexture.h>
#include <qtclasshelpermacros.h>

#include "dmabuf.h"
#include "qsg.h"

namespace ns::wayland::buffer::dmabuf {
class WlDmaBufferQSGTexture : public WlBufferQSGTexture {
public:
  ~WlDmaBufferQSGTexture() override;
  Q_DISABLE_COPY_MOVE(WlDmaBufferQSGTexture)

  [[nodiscard]] QSGTexture *texture() const override {
    return this->qsgTexture;
  }

private:
  WlDmaBufferQSGTexture(EGLImage    eglImage,
                        GLuint      glTexture,
                        QSGTexture *qsgTexture)
      : eglImage(eglImage),
        glTexture(glTexture),
        qsgTexture(qsgTexture) {}

  EGLImage    eglImage;
  GLuint      glTexture;
  QSGTexture *qsgTexture;

  friend class WlDmaBuffer;
};
} // namespace ns::wayland::buffer::dmabuf
