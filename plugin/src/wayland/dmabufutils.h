#pragma once

#include <array>
#include <cstdint>

#include <EGL/egl.h>
#include <libdrm/drm_fourcc.h>
#include <qloggingcategory.h>
#include <vulkan/vulkan_core.h>
#include <xf86drm.h>

namespace ns::wayland::buffer::dmabuf {
Q_DECLARE_LOGGING_CATEGORY(logNSDmabuf)

struct SupportedDrmFormat {
  uint32_t drmFormat;
  VkFormat vkFormat;
};

// Straight up taken from
// https://git.outfoxxed.me/quickshell/quickshell/src/branch/master/src/wayland/buffer/dmabuf.cpp
constexpr std::array<SupportedDrmFormat, 11> SUPPORTED_VK_FORMATS = {
    {
     {.drmFormat = DRM_FORMAT_ARGB8888,
         .vkFormat  = VK_FORMAT_B8G8R8A8_UNORM},
     {.drmFormat = DRM_FORMAT_XRGB8888,
         .vkFormat  = VK_FORMAT_B8G8R8A8_UNORM},
     {.drmFormat = DRM_FORMAT_ABGR8888,
         .vkFormat  = VK_FORMAT_R8G8B8A8_UNORM},
     {.drmFormat = DRM_FORMAT_XBGR8888,
         .vkFormat  = VK_FORMAT_R8G8B8A8_UNORM},
     {.drmFormat = DRM_FORMAT_ARGB2101010,
         .vkFormat  = VK_FORMAT_A2R10G10B10_UNORM_PACK32},
     {.drmFormat = DRM_FORMAT_XRGB2101010,
         .vkFormat  = VK_FORMAT_A2R10G10B10_UNORM_PACK32},
     {.drmFormat = DRM_FORMAT_ABGR2101010,
         .vkFormat  = VK_FORMAT_A2B10G10R10_UNORM_PACK32},
     {.drmFormat = DRM_FORMAT_XBGR2101010,
         .vkFormat  = VK_FORMAT_A2B10G10R10_UNORM_PACK32},
     {.drmFormat = DRM_FORMAT_ABGR16161616F,
         .vkFormat  = VK_FORMAT_R16G16B16A16_SFLOAT},
     {.drmFormat = DRM_FORMAT_RGB565,
         .vkFormat  = VK_FORMAT_R5G6B5_UNORM_PACK16},
     {.drmFormat = DRM_FORMAT_BGR565,
         .vkFormat  = VK_FORMAT_B5G6R5_UNORM_PACK16},
     }
};

VkFormat mapDrmToVkFormat(uint32_t drmFormat);
bool     drmFormatHasAlpha(uint32_t drmFormat);

template <typename T> T eglProc(const char *name) {
  auto *proc = reinterpret_cast<T>(eglGetProcAddress(name));
  if (!proc) qCCritical(logNSDmabuf) << "Failed looking up EGL proc:" << name;
  return proc;
}
} // namespace ns::wayland::buffer::dmabuf
