#include "dmabuftex_vulkan.h"

#include <qloggingcategory.h>
#include <qvulkanfunctions.h>
#include <qvulkaninstance.h>
#include <vulkan/vulkan_core.h>

namespace ns::wayland::buffer::dmabuf {
Q_DECLARE_LOGGING_CATEGORY(logNSDmabuf)

WlDmaBufferVulkanQSGTexture::~WlDmaBufferVulkanQSGTexture() {
  delete this->qsgTexture;

  if (this->image != VK_NULL_HANDLE) {
    this->devFuncs->vkDestroyImage(this->device, this->image, nullptr);
  }

  if (this->memory != VK_NULL_HANDLE) {
    this->devFuncs->vkFreeMemory(this->device, this->memory, nullptr);
  }

  qCDebug(logNSDmabuf) << "WlDmaBufferVulkanQSGTexture" << this << "destroyed";
}
} // namespace ns::wayland::buffer::dmabuf
