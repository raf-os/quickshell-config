#pragma once

#include <memory>

#include <qcontainerfwd.h>
#include <qquickwindow.h>
#include <qsgimagenode.h>
#include <qsgnode.h>
#include <qsgtexture.h>
#include <qtclasshelpermacros.h>

#include "wlbuffer.h"

namespace ns::wayland::buffer {
/** Abstract class */
class WlBufferQSGTexture {
public:
  virtual ~WlBufferQSGTexture() = default;
  Q_DISABLE_COPY_MOVE(WlBufferQSGTexture)

  [[nodiscard]] virtual QSGTexture *texture() const = 0;

  virtual void sync(const WlBuffer * /*buffer*/,
                    QQuickWindow * /*window*/) {}

protected:
  WlBufferQSGTexture() = default;
};

class WlBufferQSGDisplayNode : public QSGTransformNode {
public:
  explicit WlBufferQSGDisplayNode(QQuickWindow *window);

  void syncSwapchain(const WlBufferSwapchain &swapchain);
  void setRect(const QRectF &rect);
  void setFiltering(QSGTexture::Filtering filtering);

private:
  QQuickWindow                                          *window;
  QSGImageNode                                          *imageNode;
  QPair<WlBuffer *, std::unique_ptr<WlBufferQSGTexture>> buffer1;
  QPair<WlBuffer *, std::unique_ptr<WlBufferQSGTexture>> buffer2;
  bool presentSecondBuffer = false;
};
} // namespace ns::wayland::buffer
