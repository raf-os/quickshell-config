#include "dmabuftex_opengl.h"

#include <EGL/egl.h>
#include <GL/gl.h>
#include <qloggingcategory.h>
#include <qopenglcontext.h>
#include <qopenglcontext_platform.h>

namespace ns::wayland::buffer::dmabuf {
Q_DECLARE_LOGGING_CATEGORY(logNSDmabuf) // from dmabuf.cpp

WlDmaBufferQSGTexture::~WlDmaBufferQSGTexture() {
  auto *context = QOpenGLContext::currentContext();
  auto *display =
      context->nativeInterface<QNativeInterface::QEGLContext>()->display();

  if (this->glTexture) glDeleteTextures(1, &this->glTexture);
  if (this->eglImage) eglDestroyImage(display, this->eglImage);
  delete this->qsgTexture;

  qCDebug(logNSDmabuf) << "WlDmaBufferQSGTexture" << this << "destroyed.";
}
} // namespace ns::wayland::buffer::dmabuf
