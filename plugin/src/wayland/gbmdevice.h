#pragma once

#include <string>

#include <gbm.h>
#include <qtclasshelpermacros.h>
#include <sys/types.h>

namespace ns::wayland::buffer::dmabuf {
struct GbmDevice {
  GbmDevice(dev_t       handle,
            std::string renderNode,
            gbm_device *device)
      : handle(handle),
        renderNode(std::move(renderNode)),
        device(device) {};
  ~GbmDevice();

  Q_DISABLE_COPY_MOVE(GbmDevice)

  dev_t       handle = 0;
  std::string renderNode;
  gbm_device *device = nullptr;
};
} // namespace ns::wayland::buffer::dmabuf
