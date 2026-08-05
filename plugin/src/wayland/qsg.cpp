#include "qsg.h"

#include <QtCore>
#include <qmatrix4x4.h>
#include <qnamespace.h>
#include <qquickwindow.h>
#include <qsgtexture.h>

#include "wlbuffer.h"

namespace ns::wayland::buffer {
WlBufferQSGDisplayNode::WlBufferQSGDisplayNode(QQuickWindow *window)
    : window(window),
      imageNode(window->createImageNode()) {
  this->appendChildNode(this->imageNode);
}

void WlBufferQSGDisplayNode::setRect(const QRectF &rect) {
  const auto *buffer =
      (this->presentSecondBuffer ? this->buffer2 : this->buffer1).first;
  if (!buffer) return;

  auto matrix  = QMatrix4x4();
  auto center  = rect.center();
  auto centerX = static_cast<float>(center.x());
  auto centerY = static_cast<float>(center.y());

  // Rotates around the center of the rectangle
  matrix.translate(centerX, centerY);
  buffer->transform.apply(matrix);
  matrix.translate(-centerX, -centerY);

  auto viewRect   = matrix.mapRect(rect);
  auto bufferSize = buffer->size().toSizeF();

  bufferSize.scale(viewRect.width(), viewRect.height(), Qt::KeepAspectRatio);

  this->imageNode->setRect(
      viewRect.x() + viewRect.width() / 2 - bufferSize.width() / 2,
      viewRect.y() + viewRect.height() / 2 - bufferSize.height() / 2,
      bufferSize.width(),
      bufferSize.height());

  this->setMatrix(matrix);
}

void WlBufferQSGDisplayNode::setFiltering(QSGTexture::Filtering filtering) {
  this->imageNode->setFiltering(filtering);
}

void WlBufferQSGDisplayNode::syncSwapchain(const WlBufferSwapchain &swapchain) {
  auto *buffer = swapchain.frontBuffer();
  auto &texture =
      swapchain.shouldPresentSecondBuffer() ? this->buffer2 : this->buffer1;

  // already synced
  if (swapchain.shouldPresentSecondBuffer() == this->presentSecondBuffer &&
      texture.first == buffer) {
    return;
  }

  this->presentSecondBuffer = swapchain.shouldPresentSecondBuffer();

  if (texture.first == buffer) {
    texture.second->sync(texture.first, this->window);
  } else {
    texture.first = buffer;
    texture.second.reset(buffer->createQsgTexture(this->window));
  }

  this->imageNode->setTexture(texture.second->texture());
}
} // namespace ns::wayland::buffer
