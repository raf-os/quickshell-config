#include "dmabufutils.h"

#include <algorithm>
#include <cstdint>

#include <vulkan/vulkan_core.h>

namespace ns::wayland::buffer::dmabuf {
VkFormat mapDrmToVkFormat(uint32_t drmFormat) {
  const auto *it = std::ranges::find_if(
      SUPPORTED_VK_FORMATS,
      [&](const SupportedDrmFormat &f) { return f.drmFormat == drmFormat; });

  if (it == SUPPORTED_VK_FORMATS.end()) return VK_FORMAT_UNDEFINED;
  return it->vkFormat;
}

bool drmFormatHasAlpha(uint32_t drmFormat) {
  switch (drmFormat) {
  case DRM_FORMAT_ARGB8888:
  case DRM_FORMAT_ABGR8888:
  case DRM_FORMAT_ARGB2101010:
  case DRM_FORMAT_ABGR2101010:
  case DRM_FORMAT_ABGR16161616F: return true;
  default:                       return false;
  }
}
} // namespace ns::wayland::buffer::dmabuf
