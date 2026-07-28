#include "linuxdmabufmanager.h"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <utility>

#include <fcntl.h>
#include <gbm.h>
#include <libdrm/drm_fourcc.h>
#include <qlogging.h>
#include <qloggingcategory.h>
#include <qpair.h>
#include <qscopeguard.h>
#include <qwaylandclientextension.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <xf86drm.h>

#include "dmabuf.h"
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
  this->initialize();

  if (this->isActive()) {
    new LinuxDmabufFeedback(this->get_default_feedback());
  }
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

WlBuffer *LinuxDmabufManager::createDmabuf(const WlBufferRequest &request) {
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

    for (const auto &[format, modifiers] : formats.formats) {}
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
  buffer->planes =
      new WlDmaBuffer::Plane[buffer->planeCount](); // TODO: use a QList
                                                    // instead?
  buffer->modifier = gbm_bo_get_modifier(bo);

  auto params = QtWayland::zwp_linux_buffer_params_v1(this->create_params());

  for (auto i = 0; i < buffer->planeCount; ++i) {
    auto &plane = buffer->planes[i];
    plane.fd    = gbm_bo_get_fd_for_plane(bo, i);

    if (plane.fd < 0) {
      qCritical(logNSDmabuf) << "Failed to get gbm_bo fd for plane" << i
                             << qt_error_string(plane.fd);
      params.destroy();
      gbm_bo_destroy(bo);
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
