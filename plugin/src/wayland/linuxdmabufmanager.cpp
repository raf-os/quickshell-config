#include "linuxdmabufmanager.h"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>
#include <fcntl.h>
#include <gbm.h>
#include <libdrm/drm_fourcc.h>
#include <qdir.h>
#include <qlogging.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qopenglcontext.h>
#include <qpair.h>
#include <qquickwindow.h>
#include <qscopeguard.h>
#include <qsgrendererinterface.h>
#include <qvulkanfunctions.h>
#include <qvulkaninstance.h>
#include <qwaylandclientextension.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <unistd.h>
#include <vulkan/vulkan_core.h>
#include <xf86drm.h>

#include "dmabuf.h"
#include "dmabufutils.h"
#include "manager_p.h"
#include "qwayland-linux-dmabuf-v1.h"
#include "wayland-linux-dmabuf-v1-client-protocol.h"
#include "wlbuffer.h"
#include "wlbufferrequest.h"

namespace ns::wayland::buffer::dmabuf {
Q_DECLARE_LOGGING_CATEGORY(logNSDmabuf) // from dmabuf.cpp

namespace {
LinuxDmabufManager *MANAGER = nullptr;
}

// Makes sure ARGB is at the top, followed by XRGB. Unlike XRGB, ARGB has
// explicitly defined alpha bytes. Like much of this code, credits to
// quickshell.
// https://git.outfoxxed.me/quickshell/quickshell/src/branch/master/src/wayland/buffer/dmabuf.cpp
void LinuxDmabufFormatSelection::ensureSorted() {
  if (this->sorted) return;
  auto beginIt = this->formats.begin();

  auto argbIt = std::ranges::find_if(this->formats, [](const auto &format) {
    return format.first == DRM_FORMAT_ARGB8888;
  });

  if (argbIt != this->formats.end()) {
    std::swap(*beginIt, *argbIt);
    ++beginIt;
  }

  auto xrgbIt = std::ranges::find_if(this->formats, [](const auto &format) {
    return format.first == DRM_FORMAT_XRGB8888;
  });

  if (xrgbIt != this->formats.end()) {
    std::swap(*beginIt, *xrgbIt);
  }

  this->sorted = true;
}

LinuxDmabufFeedback::LinuxDmabufFeedback(
    ::zwp_linux_dmabuf_feedback_v1 *feedback)
    : zwp_linux_dmabuf_feedback_v1(feedback) {}

LinuxDmabufFeedback::~LinuxDmabufFeedback() { this->destroy(); }

void LinuxDmabufFeedback::zwp_linux_dmabuf_feedback_v1_main_device(
    wl_array *device) {
  if (device->size != sizeof(dev_t)) {
    qCFatal(logNSDmabuf) << "dev_t size mismatch, todo: this";
  }

  this->mainDevice = *reinterpret_cast<dev_t *>(device->data);
};

void LinuxDmabufFeedback::zwp_linux_dmabuf_feedback_v1_format_table(
    int32_t  fd,
    uint32_t size) {
  this->formatTableSize = size;
  this->formatTable     = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);

  if (this->formatTable == MAP_FAILED) {
    this->formatTable = nullptr;
    qCFatal(logNSDmabuf) << "Failed to mmap format table.";
  }
}

void LinuxDmabufFeedback::zwp_linux_dmabuf_feedback_v1_tranche_target_device(
    wl_array *device) {
  if (device->size != sizeof(dev_t)) {
    qCFatal(logNSDmabuf) << "dev_t size mismatch, todo: this";
  }

  auto &tranche  = this->tranches.emplaceBack();
  tranche.device = *reinterpret_cast<dev_t *>(device->data);
}

void LinuxDmabufFeedback::zwp_linux_dmabuf_feedback_v1_tranche_flags(
    uint32_t flags) {
  this->tranches.back().flags = flags;
}

void LinuxDmabufFeedback::zwp_linux_dmabuf_feedback_v1_tranche_formats(
    wl_array *indices) {
  struct FormatTableEntry {
    uint32_t format;
    uint32_t padding;
    uint64_t modifier;
  };

  static_assert(sizeof(FormatTableEntry) == 16,
                "Format table entry was not packed to 16 bytes.");

  if (this->formatTable == nullptr) {
    qCFatal(logNSDmabuf) << "Received tranche formats before format table.";
  }

  auto &tranche    = this->tranches.back();
  auto *table      = reinterpret_cast<FormatTableEntry *>(this->formatTable);
  auto *indexTable = reinterpret_cast<uint16_t *>(indices->data);
  auto  indexTableLength = indices->size / sizeof(uint16_t);

  uint32_t              lastFormat = 0;
  LinuxDmabufModifiers *modifiers  = nullptr;

  for (uint16_t ti = 0; ti != indexTableLength; ++ti) {
    auto        i     = indexTable[ti];
    const auto &entry = table[i];

    if (!modifiers || entry.format != lastFormat) {
      lastFormat = entry.format;
      auto modIt =
          std::ranges::find_if(tranche.formats.formats, [&](const auto &pair) {
            return pair.first == entry.format;
          });

      if (modIt == tranche.formats.formats.end()) {
        tranche.formats.formats.push_back(
            qMakePair(entry.format, LinuxDmabufModifiers()));
        modifiers = &tranche.formats.formats.back()
                         .second; // looks good but might break
      } else {
        modifiers = &modIt->second;
      }
    }

    if (entry.modifier == DRM_FORMAT_MOD_INVALID) {
      modifiers->implicit = true;
    } else {
      modifiers->modifiers.push_back(entry.modifier);
    }
  }
}

void LinuxDmabufFeedback::zwp_linux_dmabuf_feedback_v1_tranche_done() {
  //
}

void LinuxDmabufFeedback::zwp_linux_dmabuf_feedback_v1_done() {
  if (this->formatTable) {
    auto succ = munmap(this->formatTable, this->formatTableSize);
    if (succ < 0) {
      qCWarning(logNSDmabuf) << "Failed unmapping formatTable.";
    }
    this->formatTable = nullptr;
  }

  MANAGER->tranches = this->tranches;
  this->tranches.clear();

  MANAGER->feedbackDone();
}

LinuxDmabufManager::LinuxDmabufManager(WlBufferManagerPrivate *manager)
    : QWaylandClientExtensionTemplate(5),
      manager(manager) {
  MANAGER = this;
  this->initialize();

  if (this->isActive()) {
    new LinuxDmabufFeedback(this->get_default_feedback());
  }
}

LinuxDmabufManager *LinuxDmabufManager::getManager() { return MANAGER; }

bool LinuxDmabufManager::initRenderFormats(QQuickWindow *window) {
  auto *ri = window->rendererInterface();
  if (ri->graphicsApi() == QSGRendererInterface::Vulkan) {
    return this->initRenderFormatsVk(window);
  } else if (ri->graphicsApi() == QSGRendererInterface::OpenGL) {
    return this->initRenderFormatsGl(window);
  } else {
    return false;
  }
}

bool LinuxDmabufManager::initRenderFormatsVk(QQuickWindow *window) {
  qCDebug(logNSDmabuf) << "Initializing Vulkan render formats...";

  auto *ri     = window->rendererInterface();
  auto *vkInst = window->vulkanInstance();

  if (!vkInst) {
    qCCritical(logNSDmabuf)
        << "Failed to query render formats: no QVulkanInstance.";
    return false;
  }

  auto *vkDevicePtr = static_cast<VkDevice *>(
      ri->getResource(window, QSGRendererInterface::DeviceResource));
  auto *vkPhysDevicePtr = static_cast<VkPhysicalDevice *>(
      ri->getResource(window, QSGRendererInterface::PhysicalDeviceResource));

  if (!vkDevicePtr || !vkPhysDevicePtr) {
    qCCritical(logNSDmabuf)
        << "Failed to query render formats: could not get Vulkan device.";
    return false;
  }

  auto *physDevice = *vkPhysDevicePtr;
  auto *instFuncs  = vkInst->functions();

  auto drmProps = VkPhysicalDeviceDrmPropertiesEXT{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRM_PROPERTIES_EXT};

  auto props2 = VkPhysicalDeviceProperties2{
      .sType      = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
      .pNext      = &drmProps,
      .properties = {}};

  instFuncs->vkGetPhysicalDeviceProperties2(physDevice, &props2);

  auto devDri     = QDir("/dev/dri");
  auto entries    = devDri.entryList(QDir::System);
  auto findNodeFn = [&](uint32_t maj, uint32_t min) -> QString {
    for (const QString &file : entries) {
      auto        path = devDri.filePath(file);
      struct stat st   = {};
      if (::stat(path.toLocal8Bit().constData(), &st) != 0) continue;
      if (major(st.st_rdev) == maj && minor(st.st_rdev) == min) {
        return path;
      }
    }
    return QString();
  };

  QString node;
  if (drmProps.hasRender)
    node = findNodeFn(drmProps.renderMajor, drmProps.renderMinor);
  else if (drmProps.hasPrimary) {
    node = findNodeFn(drmProps.primaryMajor, drmProps.primaryMinor);
  }

  if (!node.isEmpty()) {
    this->renderNode = node.toLocal8Bit();
    qCDebug(logNSDmabuf)
        << "LinuxDmabufManager::initRenderFormatsVk: found render node"
        << this->renderNode;
  }

  for (const auto &format : SUPPORTED_VK_FORMATS) {
    VkDrmFormatModifierPropertiesListEXT modList{
        .sType = VK_STRUCTURE_TYPE_DRM_FORMAT_MODIFIER_PROPERTIES_LIST_EXT,
        .pNext = nullptr,
        .drmFormatModifierCount       = 0,
        .pDrmFormatModifierProperties = nullptr};

    VkFormatProperties2 props2{.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2,
                               .pNext = &modList,
                               .formatProperties = {}};

    instFuncs->vkGetPhysicalDeviceFormatProperties2(
        physDevice, format.vkFormat, &props2);
    if (modList.drmFormatModifierCount == 0) continue;

    auto vkMods = std::vector<VkDrmFormatModifierPropertiesEXT>(
        modList.drmFormatModifierCount);
    modList.pDrmFormatModifierProperties = vkMods.data();

    instFuncs->vkGetPhysicalDeviceFormatProperties2(
        physDevice, format.vkFormat, &props2);

    qCDebug(logNSDmabuf) << "  Format" << FourCCStr(format.drmFormat);

    LinuxDmabufModifiers mods;

    for (const auto &m : vkMods) {
      VkPhysicalDeviceImageDrmFormatModifierInfoEXT modInfo{
          .sType =
              VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_DRM_FORMAT_MODIFIER_INFO_EXT,
          .pNext                 = nullptr,
          .drmFormatModifier     = m.drmFormatModifier,
          .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
          .queueFamilyIndexCount = 0,
          .pQueueFamilyIndices   = nullptr};

      VkPhysicalDeviceImageFormatInfo2 info{
          .sType  = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2,
          .pNext  = &modInfo,
          .format = format.vkFormat,
          .type   = VK_IMAGE_TYPE_2D,
          .tiling = VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT,
          .usage  = VK_IMAGE_USAGE_SAMPLED_BIT,
          .flags  = {}};

      VkImageFormatProperties2 props = {
          .sType                 = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2,
          .pNext                 = nullptr,
          .imageFormatProperties = {}};

      auto res = instFuncs->vkGetPhysicalDeviceImageFormatProperties2(
          physDevice, &info, &props);

      if (res == VK_SUCCESS) {
        mods.modifiers.push_back(m.drmFormatModifier);
        qCDebug(logNSDmabuf)
            << "\tExplicit modifier" << FourCCModStr(m.drmFormatModifier);
      } else if (res == VK_ERROR_FORMAT_NOT_SUPPORTED) {
        qCDebug(logNSDmabuf)
            << "\tExplicit modifier" << FourCCModStr(m.drmFormatModifier)
            << "(not usable)";
      } else {
        qCDebug(logNSDmabuf)
            << "\tExplicit modifier" << FourCCModStr(m.drmFormatModifier)
            << "(unknown error)";
      }
    }

    if (mods.modifiers.isEmpty()) continue;

    // According to quickshell, implicit modifiers do not work in this
    // implementation
    // TODO: Look into this
    this->renderFormats.formats.push_back(qMakePair(format.drmFormat, mods));
  }

  return true;
}

bool LinuxDmabufManager::initRenderFormatsGl(QQuickWindow *window) {
  qCDebug(logNSDmabuf) << "Initializing OpenGL render formats...";
  auto *ri      = window->rendererInterface();
  auto *context = static_cast<QOpenGLContext *>(
      ri->getResource(window, QSGRendererInterface::OpenGLContextResource));

  if (!context) {
    qCCritical(logNSDmabuf) << "Failed to query render formats: No GL context.";
    return false;
  }

  auto *qEglContext = context->nativeInterface<QNativeInterface::QEGLContext>();
  if (!qEglContext) {
    qCCritical(logNSDmabuf)
        << "Failed to query render formats: No EGL context.";
    return false;
  }

  auto display = qEglContext->display();

  static auto *eglQueryDisplayAttribEXT =
      eglProc<PFNEGLQUERYDISPLAYATTRIBEXTPROC>("eglQueryDisplayAttribEXT");
  static auto *eglQueryDeviceStringEXT =
      eglProc<PFNEGLQUERYDEVICESTRINGEXTPROC>("eglQueryDeviceStringEXT");
  static auto *eglQueryDmaBufFormatsEXT =
      eglProc<PFNEGLQUERYDMABUFFORMATSEXTPROC>("eglQueryDmaBufFormatsEXT");
  static auto *eglQueryDmaBufModifiersEXT =
      eglProc<PFNEGLQUERYDMABUFMODIFIERSEXTPROC>("eglQueryDmaBufModifiersEXT");

  if (!eglQueryDisplayAttribEXT || !eglQueryDeviceStringEXT ||
      !eglQueryDmaBufFormatsEXT || !eglQueryDmaBufModifiersEXT) {
    return false;
  }

  EGLAttrib deviceAttrib = 0;
  if (!eglQueryDisplayAttribEXT(display, EGL_DEVICE_EXT, &deviceAttrib)) {
    qCCritical(logNSDmabuf)
        << "Failed to find render device: device display attrib missing.";
    return false;
  }

  auto *dev = reinterpret_cast<EGLDeviceEXT>(deviceAttrib);

  if (const auto *renderNode =
          eglQueryDeviceStringEXT(dev, EGL_DRM_RENDER_NODE_FILE_EXT)) {
    this->renderNode = renderNode;
  } else if (const auto *primaryNode =
                 eglQueryDeviceStringEXT(dev, EGL_DRM_DEVICE_FILE_EXT)) {
    this->renderNode = primaryNode;
  } else {
    qCCritical(logNSDmabuf)
        << "Failed to find render device: no render or primary node found.";
    return false;
  }

  qCDebug(logNSDmabuf) << "Found render node:" << this->renderNode;

  EGLint numFormats = 0;
  if (!eglQueryDmaBufFormatsEXT(display, 0, nullptr, &numFormats)) {
    qCCritical(logNSDmabuf)
        << "Failed to query render formats: eglQueryDmaBufFormatsEXT failed.";
    return false;
  }

  if (numFormats == 0) {
    qCCritical(logNSDmabuf) << "Failed to query render formats: zero formats.";
    return false;
  }

  auto formats = std::vector<EGLint>(numFormats);
  if (!eglQueryDmaBufFormatsEXT(
          display, numFormats, formats.data(), &numFormats)) {
    qCCritical(logNSDmabuf)
        << "Failed to query render formats: eglQueryDmaBufFormatsEXT failed.";
    return false;
  }

  qCDebug(logNSDmabuf) << "Render formats:";
  for (auto format : formats) {
    qCDebug(logNSDmabuf) << "  [FORMAT]" << FourCCStr(format);
    qCDebug(logNSDmabuf) << "\tImplicit modifier";

    EGLint numModifiers = 0;
    if (!eglQueryDmaBufModifiersEXT(
            display, format, 0, nullptr, nullptr, &numModifiers)) {
      qCCritical(logNSDmabuf) << "Failed to query render formats: "
                                 "eglQueryDmaBufModifiersEXT failed.";
      return false;
    }

    auto modifiers    = std::vector<EGLuint64KHR>(numModifiers);
    auto externalOnly = std::vector<EGLBoolean>(numModifiers);
    if (!eglQueryDmaBufModifiersEXT(display,
                                    format,
                                    numModifiers,
                                    modifiers.data(),
                                    externalOnly.data(),
                                    &numModifiers)) {
      qCCritical(logNSDmabuf) << "Failed to query render formats: "
                                 "eglQueryDmaBufModifiersEXT failed.";
      return false;
    }

    LinuxDmabufModifiers mods;
    for (size_t i = 0; i != modifiers.size(); i++) {
      // NOTE: No support for importing external-only modifiers, required for
      // some MGPU cases. Will fall back to SHM.
      // TODO: Look into this
      auto external = externalOnly[i] == EGL_TRUE;
      auto modifier = modifiers[i];

      if (external) {
        qCDebug(logNSDmabuf) << "\tExplicit modifier" << FourCCModStr(modifier)
                             << "(external / skipped)";
      } else {
        mods.modifiers.push_back(modifier);
        qCDebug(logNSDmabuf) << "\tExplicit modifier" << FourCCModStr(modifier);
      }
    }

    mods.implicit = true;
    this->renderFormats.formats.push_back(qMakePair(format, std::move(mods)));
  }

  return true;
}

void LinuxDmabufManager::feedbackDone() { this->manager->dmabufReady(); }

std::shared_ptr<GbmDevice> LinuxDmabufManager::getGbmDevice(dev_t handle) {
  for (const auto &weak : this->gbmDevices) {
    auto shared = weak.lock();
    if (shared->handle == handle) {
      return shared;
    }
  }

  drmDevice *drmDev = nullptr;
  if (auto error = drmGetDeviceFromDevId(handle, 0, &drmDev); error != 0) {
    qCWarning(logNSDmabuf)
        << "Failed to get drm device information from handle:"
        << qt_error_string(error);
    return nullptr;
  }

  auto drmDevGuard = qScopeGuard([&] { drmFreeDevice(&drmDev); });

  std::string renderNodeStorage;

  if (drmDev->available_nodes & (1 << DRM_NODE_RENDER)) {
    renderNodeStorage = drmDev->nodes[DRM_NODE_RENDER];
  } else if (drmDev->available_nodes & (1 << DRM_NODE_PRIMARY)) {
    renderNodeStorage = drmDev->nodes[DRM_NODE_PRIMARY];
  } else {
    qCDebug(logNSDmabuf)
        << "Cannot create GBM device: DRM device does not have render node.";
    return nullptr;
  }

  for (const auto &weak : this->gbmDevices) {
    auto shared = weak.lock();
    if (shared->renderNode == renderNodeStorage) {
      return shared;
    }
  }

  auto fd = open(renderNodeStorage.c_str(), O_RDWR | O_CLOEXEC);
  if (fd < 0) {
    qCDebug(logNSDmabuf) << "Unable to open render node"
                         << renderNodeStorage.c_str() << ":"
                         << qt_error_string(fd);
    return nullptr;
  }

  auto *device = gbm_create_device(fd);
  if (!device) {
    qCDebug(logNSDmabuf) << "Failed to create GBM device from render node"
                         << renderNodeStorage.c_str();
    close(fd);
    return nullptr;
  }

  auto shared =
      std::make_shared<GbmDevice>(handle, std::move(renderNodeStorage), device);
  this->gbmDevices.push_back(shared);
  return shared;
}

WlBuffer *
LinuxDmabufManager::createDmabufFromRequest(const WlBufferRequest &request) {
  for (auto &tranche : this->tranches) {
    if (request.dmabuf.device != 0 && tranche.device != request.dmabuf.device) {
      continue;
    }

    LinuxDmabufFormatSelection formats;
    for (const auto &requestFormat : request.dmabuf.formats) {
      for (const auto &[renderFormat, renderFormatModifiers] :
           this->renderFormats.formats) {
        if (renderFormat != requestFormat.format) continue;

        if (!requestFormat.modifiers.isEmpty()) {
          LinuxDmabufModifiers mods;
          mods.implicit =
              requestFormat.implicit && renderFormatModifiers.implicit;

          for (auto mod : requestFormat.modifiers) {
            for (auto renderMod : renderFormatModifiers.modifiers) {
              if (mod != renderMod) continue;
              mods.modifiers.push_back(mod);
              break;
            }
          }

          if (mods.implicit || !mods.modifiers.isEmpty()) {
            formats.formats.push_back(qMakePair(requestFormat.format, mods));
          }
        } else {
          for (const auto &[trancheFormat, trancheMods] :
               tranche.formats.formats) {
            if (trancheFormat != requestFormat.format) continue;

            LinuxDmabufModifiers mods;
            mods.implicit =
                trancheMods.implicit && renderFormatModifiers.implicit;

            for (auto mod : trancheMods.modifiers) {
              for (auto renderMod : renderFormatModifiers.modifiers) {
                if (mod != renderMod) continue;
                mods.modifiers.push_back(mod);
                break;
              }
            }

            if (mods.implicit || !mods.modifiers.isEmpty()) {
              formats.formats.push_back(qMakePair(trancheFormat, mods));
            }

            break;
          }
        }

        break;
      }
    }

    if (formats.formats.isEmpty()) continue;
    formats.ensureSorted();

    auto gbmDevice = this->getGbmDevice(tranche.device);

    if (!gbmDevice) {
      qCWarning(logNSDmabuf)
          << "Unable to create dmabuf - unusable tranche device was provided.";
      continue;
    }

    for (const auto &[format, modifiers] : formats.formats) {
      if (auto *buf = this->createDmabuf(
              gbmDevice, format, modifiers, request.width, request.height)) {
        return buf;
      }
    }
  }

  qCWarning(logNSDmabuf) << "Unable to create dmabuf - no matching formats.";
  return nullptr;
}

WlBuffer *
LinuxDmabufManager::createDmabuf(const std::shared_ptr<GbmDevice> &device,
                                 uint32_t                          format,
                                 const LinuxDmabufModifiers       &modifiers,
                                 uint32_t                          width,
                                 uint32_t                          height) {
  auto  buffer = std::unique_ptr<WlDmaBuffer>(new WlDmaBuffer());
  auto &bo     = buffer->bo;

  const uint32_t flags = GBM_BO_USE_RENDERING;

  if (modifiers.modifiers.isEmpty()) {
    if (!modifiers.implicit) {
      qCritical(logNSDmabuf) << "Unable to create gbm_bo: format supports no "
                                "implicit or explicit modifiers.";
      return nullptr;
    }

    if (device->renderNode != this->renderNode) {
      qCritical(logNSDmabuf)
          << "Unable to create gbm_bo: format supports only implicit modifier "
             "which does not work accross GPUs.";
      return nullptr;
    }

    qCDebug(logNSDmabuf) << "Creating gbm_bo without modifiers...";
    buffer->usedImplicitModifier = true;
    bo = gbm_bo_create(device->device, width, height, format, flags);
  } else {
    qCDebug(logNSDmabuf) << "Creating gbm_bo with modifiers...";

    bo = gbm_bo_create_with_modifiers2(device->device,
                                       width,
                                       height,
                                       format,
                                       modifiers.modifiers.data(),
                                       modifiers.modifiers.length(),
                                       flags);
  }

  if (!bo) {
    qCritical(logNSDmabuf) << "Failed creating gbm_bo.";
    return nullptr;
  }

  buffer->planeCount = gbm_bo_get_plane_count(bo);
  // buffer->planes   = new WlDmaBuffer::Plane[buffer->planeCount]();
  // using std::vector instead of raw VLA
  buffer->planes   = std::vector<WlDmaBuffer::Plane>(buffer->planeCount);
  buffer->modifier = gbm_bo_get_modifier(bo);

  auto params = QtWayland::zwp_linux_buffer_params_v1(this->create_params());

  for (auto i = 0; i < buffer->planeCount; ++i) {
    auto &plane = buffer->planes[i];
    plane.fd    = gbm_bo_get_fd_for_plane(bo, i);

    if (plane.fd < 0) {
      qCritical(logNSDmabuf) << "Failed to get gbm_bo fd for plane" << i
                             << qt_error_string(plane.fd);
      params.destroy();
      return nullptr;
    }

    plane.stride = gbm_bo_get_stride_for_plane(bo, i);
    plane.offset = gbm_bo_get_offset(bo, i);

    params.add(plane.fd,
               i,
               plane.offset,
               plane.stride,
               buffer->modifier >> 32,
               buffer->modifier & 0xffffffff);
  }

  buffer->m_buffer = params.create_immed(
      static_cast<int32_t>(width), static_cast<int32_t>(height), format, 0);
  params.destroy();

  buffer->device = device;
  buffer->width  = width;
  buffer->height = height;
  buffer->format = format;

  qCDebug(logNSDmabuf) << "Created dmabuf" << buffer.get();
  return buffer.release();
}
} // namespace ns::wayland::buffer::dmabuf
