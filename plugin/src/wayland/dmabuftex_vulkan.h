#pragma once

#include <qsgtexture.h>
#include <qtclasshelpermacros.h>
#include <qvulkaninstance.h>
#include <vulkan/vulkan_core.h>

#include "dmabuf.h"
#include "qsg.h"

namespace ns::wayland::buffer::dmabuf {
class WlDmaBufferVulkanQSGTexture : public WlBufferQSGTexture {
public:
  ~WlDmaBufferVulkanQSGTexture() override;
  Q_DISABLE_COPY_MOVE(WlDmaBufferVulkanQSGTexture)

  [[nodiscard]] QSGTexture *texture() const override {
    return this->qsgTexture;
  }

private:
  WlDmaBufferVulkanQSGTexture(QVulkanDeviceFunctions *devFuncs,
                              VkDevice                device,
                              VkImage                 image,
                              VkDeviceMemory          memory,
                              QSGTexture             *qsgTexture)
      : devFuncs(devFuncs),
        device(device),
        image(image),
        memory(memory),
        qsgTexture(qsgTexture) {}

  QVulkanDeviceFunctions *devFuncs   = nullptr;
  VkDevice                device     = VK_NULL_HANDLE;
  VkImage                 image      = VK_NULL_HANDLE;
  VkDeviceMemory          memory     = VK_NULL_HANDLE;
  QSGTexture             *qsgTexture = nullptr;

  friend class WlDmaBuffer;
};
} // namespace ns::wayland::buffer::dmabuf
