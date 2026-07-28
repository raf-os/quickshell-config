#include "qsg.h"

#include <QtCore>
#include <qquickwindow.h>
#include <qsgtexture.h>

#include "wlbuffer.h"

namespace ns::wayland::buffer {
WlBufferQSGDisplayNode::WlBufferQSGDisplayNode(QQuickWindow *window)
    : window(window),
      imageNode(window->createImageNode()) {
  this->appendChildNode(this->imageNode);
}

void WlBufferQSGDisplayNode::setRect(const QRectF &rect) {}

void WlBufferQSGDisplayNode::setFiltering(QSGTexture::Filtering filtering) {
  this->imageNode->setFiltering(filtering);
}

void WlBufferQSGDisplayNode::syncSwapchain(const WlBufferSwapchain &swapchain) {
}
} // namespace ns::wayland::buffer
