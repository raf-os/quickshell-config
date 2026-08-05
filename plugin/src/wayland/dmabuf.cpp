#include "dmabuf.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>
#include <GL/gl.h>
#include <gbm.h>
#include <libdrm/drm_fourcc.h>
#include <private/qquickwindow_p.h>
#include <private/qrhivulkan_p.h>
#include <qdebug.h>
#include <qloggingcategory.h>
#include <qopenglcontext.h>
#include <qopenglcontext_platform.h>
#include <qquickwindow.h>
#include <qscopeguard.h>
#include <qsgrendererinterface.h>
#include <qsgtexture_platform.h>
#include <qsize.h>
#include <qvulkanfunctions.h>
#include <unistd.h>
#include <vulkan/vulkan_core.h>
#include <wayland-client-protocol.h>

#include "dmabuftex_opengl.h"
#include "dmabuftex_vulkan.h"
#include "dmabufutils.h"
#include "qsg.h"
#include "wlbufferrequest.h"

namespace ns::wayland::buffer::dmabuf {
Q_LOGGING_CATEGORY(logNSDmabuf,
                   "nightshell.wayland.buffer.dmabuf")

namespace {}

QDebug &operator<<(QDebug          &debug,
                   const FourCCStr &fourcc) {
  debug << fourcc.cStr();
  return debug;
}
QDebug &operator<<(QDebug             &debug,
                   const FourCCModStr &fourcc) {
  debug << fourcc.cStr();
  return debug;
}

QDebug &operator<<(QDebug            &debug,
                   const WlDmaBuffer *buffer) {
  auto saver = QDebugStateSaver(debug);
  debug.nospace();

  if (buffer) {
    debug << "WlDmaBuffer(" << static_cast<const void *>(buffer)
          << ", size=" << buffer->width << 'x' << buffer->height
          << ", format=" << FourCCStr(buffer->format) << ", modifier=`"
          << FourCCModStr(buffer->modifier) << "`)";
  } else {
    debug << "WlDmaBuffer(0x0)";
  }

  return debug;
}

WlDmaBuffer::~WlDmaBuffer() {
  if (this->m_buffer) {
    wl_buffer_destroy(this->m_buffer);
  }

  if (this->bo) {
    gbm_bo_destroy(this->bo);
  }

  for (auto i = 0; i < this->planeCount; ++i) {
    const auto &plane = this->planes[i];
    if (plane.fd) close(plane.fd);
  }

  // delete[] this->planes;
}

WlDmaBuffer::WlDmaBuffer(WlDmaBuffer &&other) noexcept
    : device(std::move(other.device)),
      bo(other.bo),
      m_buffer(other.m_buffer),
      planes(other.planes) {
  other.m_buffer   = nullptr;
  other.bo         = nullptr;
  other.planeCount = 0;
}

WlDmaBuffer &WlDmaBuffer::operator=(WlDmaBuffer &&other) noexcept {
  this->~WlDmaBuffer();
  new (this) WlDmaBuffer(std::move(other));
  return *this;
}

bool WlDmaBuffer::isCompatible(const WlBufferRequest &request) const {
  if (request.width != this->width || request.height != this->height)
    return false;

  auto matchingFormat =
      std::ranges::find_if(request.dmabuf.formats, [this](const auto &format) {
        if (format.format != this->format) return false;
        return format.format == this->format &&
               (format.modifiers.isEmpty() ||
                (format.implicit && this->usedImplicitModifier) ||
                std::ranges::find(format.modifiers, this->modifier) !=
                    format.modifiers.end());
      });

  return matchingFormat != request.dmabuf.formats.end();
}

WlBufferQSGTexture *WlDmaBuffer::createQsgTexture(QQuickWindow *window) const {
  auto *ri = window->rendererInterface();

  if (ri && ri->graphicsApi() == QSGRendererInterface::Vulkan) {
    return this->createQsgTextureVulkan(window);
  }

  return this->createQsgTextureGl(window);
}

WlBufferQSGTexture *
WlDmaBuffer::createQsgTextureGl(QQuickWindow *window) const {
  static auto *glEGLImageTargetTexture2DOES = []() {
    auto *fn = reinterpret_cast<PFNGLEGLIMAGETARGETTEXTURE2DOESPROC>(
        eglGetProcAddress("glEGLImageTargetTexture2DOES"));

    if (!fn) {
      qCFatal(logNSDmabuf) << "Failed to create QSG texture from WlDmaBuffer: "
                              "glEGLImageTargetTexture2DOES is missing.";
    }

    return fn;
  }();

  auto *context = QOpenGLContext::currentContext();
  if (!context) {
    qCFatal(logNSDmabuf)
        << "Failed to create QSG texture from WlDmaBuffer: No GL context.";
  }

  auto *qEglContext = context->nativeInterface<QNativeInterface::QEGLContext>();
  if (!qEglContext) {
    qCFatal(logNSDmabuf)
        << "Failed to create QSG texture from WlDmaBuffer: No EGL context.";
  }

  auto *display = qEglContext->display();

  // https://github.com/hyprwm/hyprlock/blob/main/src/renderer/Screencopy.cpp#L201
  static constexpr struct {
    EGLAttrib fd;
    EGLAttrib offset;
    EGLAttrib pitch;
    EGLAttrib modlo;
    EGLAttrib modhi;
  } attrNames[4] = {
      {.fd     = EGL_DMA_BUF_PLANE0_FD_EXT,
       .offset = EGL_DMA_BUF_PLANE0_OFFSET_EXT,
       .pitch  = EGL_DMA_BUF_PLANE0_PITCH_EXT,
       .modlo  = EGL_DMA_BUF_PLANE0_MODIFIER_LO_EXT,
       .modhi  = EGL_DMA_BUF_PLANE0_MODIFIER_HI_EXT},
      {.fd     = EGL_DMA_BUF_PLANE1_FD_EXT,
       .offset = EGL_DMA_BUF_PLANE1_OFFSET_EXT,
       .pitch  = EGL_DMA_BUF_PLANE1_PITCH_EXT,
       .modlo  = EGL_DMA_BUF_PLANE1_MODIFIER_LO_EXT,
       .modhi  = EGL_DMA_BUF_PLANE1_MODIFIER_HI_EXT},
      {.fd     = EGL_DMA_BUF_PLANE2_FD_EXT,
       .offset = EGL_DMA_BUF_PLANE2_OFFSET_EXT,
       .pitch  = EGL_DMA_BUF_PLANE2_PITCH_EXT,
       .modlo  = EGL_DMA_BUF_PLANE2_MODIFIER_LO_EXT,
       .modhi  = EGL_DMA_BUF_PLANE2_MODIFIER_HI_EXT},
      {.fd     = EGL_DMA_BUF_PLANE3_FD_EXT,
       .offset = EGL_DMA_BUF_PLANE3_OFFSET_EXT,
       .pitch  = EGL_DMA_BUF_PLANE3_PITCH_EXT,
       .modlo  = EGL_DMA_BUF_PLANE3_MODIFIER_LO_EXT,
       .modhi  = EGL_DMA_BUF_PLANE3_MODIFIER_HI_EXT}
  };

  std::vector<EGLAttrib> attribs = {EGL_WIDTH,
                                    this->width,
                                    EGL_HEIGHT,
                                    this->height,
                                    EGL_LINUX_DRM_FOURCC_EXT,
                                    this->format};

  if (this->planeCount > 4) {
    qCFatal(logNSDmabuf)
        << "Unable to create EGL attrib array with more than 4 planes. Count:"
        << this->planeCount;
  }

  for (auto i = 0; i != this->planeCount; i++) {
    const auto &names = attrNames[i];
    const auto &plane = this->planes[i];

    attribs.emplace_back(names.fd);
    attribs.emplace_back(plane.fd);
    attribs.emplace_back(names.offset);
    attribs.emplace_back(plane.fd);
    attribs.emplace_back(names.pitch);
    attribs.emplace_back(plane.stride);

    if (this->modifier != DRM_FORMAT_MOD_INVALID) {
      attribs.emplace_back(names.modlo);
      attribs.emplace_back(static_cast<EGLAttrib>(this->modifier & 0xFFFFFFFF));
      attribs.emplace_back(names.modhi);
      attribs.emplace_back(static_cast<EGLAttrib>(this->modifier >> 32));
    }
  }

  attribs.emplace_back(EGL_NONE);

  auto *eglImage = eglCreateImage(
      display, EGL_NO_CONTEXT, EGL_LINUX_DMA_BUF_EXT, nullptr, attribs.data());

  if (eglImage == EGL_NO_IMAGE) {
    qCFatal(logNSDmabuf) << "Failed to create egl image" << eglGetError();
    return nullptr;
  }

  window->beginExternalCommands();
  GLuint glTexture = 0;
  glGenTextures(1, &glTexture);

  glBindTexture(GL_TEXTURE_2D, glTexture);
  glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, eglImage);
  glBindTexture(GL_TEXTURE_2D, 0);
  window->endExternalCommands();

  auto *qsgTexture = QNativeInterface::QSGOpenGLTexture::fromNative(
      glTexture,
      window,
      QSize(static_cast<int>(this->width), static_cast<int>(this->height)));
  auto *tex = new WlDmaBufferQSGTexture(eglImage, glTexture, qsgTexture);
  qCDebug(logNSDmabuf) << "Created WlDmaBufferQSGTexture" << tex << "from"
                       << this;
  return tex;
}

WlBufferQSGTexture *
WlDmaBuffer::createQsgTextureVulkan(QQuickWindow *window) const {
  // This is ripped off of quickshell, and mostly cleaned up a bit, using scope
  // guards instead of "goto"
  //
  // TODO: read up on vulkan API; check if it's too much of a pain to use
  // Vulkan-Hpp - maybe it's not even necessary and may add more crap to the
  // compilation stage, on top of having an extra Vulkan SDK dependency. Plus, a
  // lot of the cpp wrapper raii methods take ownership of the created
  // structures which is a big no-no considering Qt is the one managing vulkan,
  // and it may also throw exceptions, which qt does not like much at all
  //
  // https://git.outfoxxed.me/quickshell/quickshell/src/branch/master/src/wayland/buffer/dmabuf.cpp
  auto *ri     = window->rendererInterface();
  auto *vkInst = window->vulkanInstance();

  if (!vkInst) {
    qCWarning(logNSDmabuf)
        << "Failed to create Vulkan QSG texture: no QVulkanInstance.";
    return nullptr;
  }

  auto *vkDevicePtr = static_cast<VkDevice *>(
      ri->getResource(window, QSGRendererInterface::DeviceResource));
  auto *vkPhysDevicePtr = static_cast<VkPhysicalDevice *>(
      ri->getResource(window, QSGRendererInterface::PhysicalDeviceResource));

  if (!vkDevicePtr || !vkPhysDevicePtr) {
    qCWarning(logNSDmabuf)
        << "Failed to create Vulkan QSG texture: unable to get Vulkan device.";
    return nullptr;
  }

  VkDevice         device     = *vkDevicePtr;
  VkPhysicalDevice physDevice = *vkPhysDevicePtr;

  auto *devFuncs  = vkInst->deviceFunctions(device);
  auto *instFuncs = vkInst->functions();

  if (!devFuncs || !instFuncs) {
    qCWarning(logNSDmabuf) << "Failed to create Vulkan QSG texture: unable to "
                              "get Vulkan functions.";
    return nullptr;
  }

  auto getMemoryFdPropertiesKHR =
      reinterpret_cast<PFN_vkGetMemoryFdPropertiesKHR>(
          instFuncs->vkGetDeviceProcAddr(device, "vkGetMemoryFdPropertiesKHR"));

  if (!getMemoryFdPropertiesKHR) {
    qCWarning(logNSDmabuf)
        << "Failed to create Vulkan QSG texture: vkGetMemoryFdPropertiesKHR "
           "not available. Missing VK_KHR_external_memory_fd extension.";
    return nullptr;
  }

  const VkFormat vkFormat = mapDrmToVkFormat(this->format);
  if (vkFormat == VK_FORMAT_UNDEFINED) {
    qCWarning(logNSDmabuf)
        << "Failed to create Vulkan QSG texture: unsupported DRM format"
        << FourCCStr(this->format);
    return nullptr;
  }

  if (this->planeCount > 4) {
    qCWarning(logNSDmabuf)
        << "Attempted to create Vulkan QSG texture with too many planes:"
        << this->planeCount;
    return nullptr;
  }

  std::array<VkSubresourceLayout, 4> planeLayouts = {};
  for (auto i = 0; i < this->planeCount; ++i) {
    planeLayouts[i].offset     = this->planes[i].offset;
    planeLayouts[i].rowPitch   = this->planes[i].stride;
    planeLayouts[i].size       = 0;
    planeLayouts[i].arrayPitch = 0;
    planeLayouts[i].rowPitch   = 0;
  }

  const bool useModifier = this->modifier != DRM_FORMAT_MOD_INVALID;

  VkExternalMemoryImageCreateInfo externalInfo = {
      .sType       = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO,
      .handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT};

  VkImageDrmFormatModifierExplicitCreateInfoEXT modifierInfo = {
      .sType =
          VK_STRUCTURE_TYPE_IMAGE_DRM_FORMAT_MODIFIER_EXPLICIT_CREATE_INFO_EXT,
      .drmFormatModifier           = this->modifier,
      .drmFormatModifierPlaneCount = static_cast<uint32_t>(this->planeCount),
      .pPlaneLayouts               = planeLayouts.data()};

  if (useModifier) {
    externalInfo.pNext = &modifierInfo;
  }

  VkImageCreateInfo imageInfo = {
      .sType       = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .pNext       = &externalInfo,
      .imageType   = VK_IMAGE_TYPE_2D,
      .format      = vkFormat,
      .extent      = {.width = this->width, .height = this->height, .depth = 1},
      .mipLevels   = 1,
      .arrayLayers = 1,
      .samples     = VK_SAMPLE_COUNT_1_BIT,
      .tiling      = useModifier ? VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT
                                 : VK_IMAGE_TILING_LINEAR,
      .usage       = VK_IMAGE_USAGE_SAMPLED_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
  };

  VkImage  image = VK_NULL_HANDLE;
  VkResult result =
      devFuncs->vkCreateImage(device, &imageInfo, nullptr, &image);

  // First scope guard, will free image memory
  auto imageScopeGuard = qScopeGuard([device, devFuncs, image] {
    if (image != VK_NULL_HANDLE) {
      devFuncs->vkDestroyImage(device, image, nullptr);
    }
  });

  if (result != VK_SUCCESS) {
    qCWarning(logNSDmabuf)
        << "Failed to create VkImage for DMA-BUF import, result:" << result;
    return nullptr;
  }

  VkDeviceMemory memory = VK_NULL_HANDLE;

  // Second scope guard, will free device memory
  auto memoryScopeGuard = qScopeGuard([device, devFuncs, memory] {
    if (memory != VK_NULL_HANDLE) {
      devFuncs->vkFreeMemory(device, memory, nullptr);
    }
  });

  // Gotta duplicate the file descriptor, as vkAllocateMemory takes ownership
  const int dupFd = dup(this->planes[0].fd);
  if (dupFd < 0) {
    qCWarning(logNSDmabuf) << "Failed to dup() fd for DMA-BUF import.";
    return nullptr;
  }

  VkMemoryRequirements memReqs = {};
  devFuncs->vkGetImageMemoryRequirements(device, image, &memReqs);

  VkMemoryFdPropertiesKHR fdProps = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_FD_PROPERTIES_KHR,
  };

  result = getMemoryFdPropertiesKHR(
      device, VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT, dupFd, &fdProps);

  if (result != VK_SUCCESS) {
    close(dupFd);
    qCWarning(logNSDmabuf) << "vkGetMemoryFdPropertiesKHR failed, result:"
                           << result;
    return nullptr;
  }

  const uint32_t memTypeBits = memReqs.memoryTypeBits & fdProps.memoryTypeBits;

  VkPhysicalDeviceMemoryProperties memProps = {};
  instFuncs->vkGetPhysicalDeviceMemoryProperties(physDevice, &memProps);

  uint32_t memTypeIndex = UINT32_MAX;
  for (uint32_t j = 0; j < memProps.memoryTypeCount; ++j) {
    if (memTypeBits & (1u << j)) {
      memTypeIndex = j;
      break;
    }
  }

  if (memTypeIndex == UINT32_MAX) {
    close(dupFd);
    qCWarning(logNSDmabuf) << "No compatible memory type for DMA-BUF import.";
    return nullptr;
  }

  VkImportMemoryFdInfoKHR importInfo = {
      .sType      = VK_STRUCTURE_TYPE_IMPORT_MEMORY_FD_INFO_KHR,
      .handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT,
      .fd         = dupFd};

  VkMemoryDedicatedAllocateInfo dedicatedInfo = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO,
      .pNext = &importInfo,
      .image = image};

  VkMemoryAllocateInfo allocInfo = {.sType =
                                        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                                    .pNext           = &dedicatedInfo,
                                    .allocationSize  = memReqs.size,
                                    .memoryTypeIndex = memTypeIndex};

  result = devFuncs->vkAllocateMemory(device, &allocInfo, nullptr, &memory);
  if (result != VK_SUCCESS) {
    close(dupFd);
    qCWarning(logNSDmabuf) << "vkAllocateMemory failed, result:" << result;
    return nullptr;
  }

  result = devFuncs->vkBindImageMemory(device, image, memory, 0);
  if (result != VK_SUCCESS) {
    close(dupFd);
    qCWarning(logNSDmabuf) << "vkBindImageMemory failed, result:" << result;
    return nullptr;
  }

  window->beginExternalCommands();

  auto *cmdBufPtr = static_cast<VkCommandBuffer *>(
      ri->getResource(window, QSGRendererInterface::CommandListResource));

  if (cmdBufPtr && *cmdBufPtr) {
    VkCommandBuffer cmdBuf = *cmdBufPtr;

    uint32_t graphicsQueueFamily = 0;
    uint32_t queueFamilyCount    = 0;

    instFuncs->vkGetPhysicalDeviceQueueFamilyProperties(
        physDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

    instFuncs->vkGetPhysicalDeviceQueueFamilyProperties(
        physDevice, &queueFamilyCount, queueFamilies.data());

    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
      if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        graphicsQueueFamily = i;
        break;
      }
    }

    VkImageMemoryBarrier barrier = {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask       = 0,
        .dstAccessMask       = VK_ACCESS_SHADER_READ_BIT,
        .oldLayout           = VK_IMAGE_LAYOUT_GENERAL,
        .newLayout           = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_FOREIGN_EXT,
        .dstQueueFamilyIndex = graphicsQueueFamily,
        .image               = image,
        .subresourceRange    = {.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                                .baseMipLevel   = 0,
                                .levelCount     = 1,
                                .baseArrayLayer = 0,
                                .layerCount     = 1},
    };

    devFuncs->vkCmdPipelineBarrier(cmdBuf,
                                   VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                   VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                   0,
                                   0,
                                   nullptr,
                                   0,
                                   nullptr,
                                   1,
                                   &barrier);
  }

  window->endExternalCommands();

  auto *qsgTexture =
      QQuickWindowPrivate::get(window)->createTextureFromNativeTexture(
          reinterpret_cast<quint64>(image),
          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
          static_cast<uint>(vkFormat),
          QSize(static_cast<int>(this->width), static_cast<int>(this->height)),
          {});

  // Opaque DRM formats have undefined alpha bytes. Vulkan, as explicit and as
  // verbose as it is, does not like that. So we remap alpha to one (fully
  // opaque) for these formats
  if (!drmFormatHasAlpha(this->format)) {
    auto *vkTexture = static_cast<QVkTexture *>(qsgTexture->rhiTexture());

    devFuncs->vkDestroyImageView(device, vkTexture->imageView, nullptr);

    VkImageViewCreateInfo viewInfo = {
        .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image            = image,
        .viewType         = VK_IMAGE_VIEW_TYPE_2D,
        .format           = vkFormat,
        .components       = {.r = VK_COMPONENT_SWIZZLE_IDENTITY,
                             .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                             .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                             .a = VK_COMPONENT_SWIZZLE_ONE},
        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                             .levelCount = 1,
                             .layerCount = 1}
    };

    result = devFuncs->vkCreateImageView(
        device, &viewInfo, nullptr, &vkTexture->imageView);
    if (result != VK_SUCCESS) {
      qCWarning(logNSDmabuf)
          << "Failed to create alpha-swizzled VkImageView, result:" << result;
    }
  }

  // By now everything must've worked correctly, so we gotta dismiss the scope
  // guards to avoid catastrophic use-after-frees
  imageScopeGuard.dismiss();
  memoryScopeGuard.dismiss();

  auto *tex = new WlDmaBufferVulkanQSGTexture(
      devFuncs, device, image, memory, qsgTexture);
  return tex;
}
} // namespace ns::wayland::buffer::dmabuf
