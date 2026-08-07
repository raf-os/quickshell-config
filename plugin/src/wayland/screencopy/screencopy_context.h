#pragma once

#include <qobject.h>
#include <qtmetamacros.h>

#include "wlbuffer.h"

namespace ns::wayland::screencopy {
class ScreencopyContext : public QObject {
  Q_OBJECT

public:
  [[nodiscard]] buffer::WlBufferSwapchain &swapchain() {
    return this->m_swapchain;
  }
  virtual void captureFrame() = 0;

signals:
  void frameCaptured();
  void stopped();

protected:
  ScreencopyContext() = default;

  buffer::WlBufferSwapchain m_swapchain;
};
} // namespace ns::wayland::screencopy
