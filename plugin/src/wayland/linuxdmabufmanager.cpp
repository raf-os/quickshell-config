#include "linuxdmabufmanager.h"

#include <algorithm>
#include <cstdint>
#include <memory>

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

#include "manager_p.h"
#include "wayland-linux-dmabuf-v1-client-protocol.h"

namespace ns::wayland::buffer::dmabuf {
Q_DECLARE_LOGGING_CATEGORY(logNSDmabuf) // from dmabuf.cpp

namespace {
LinuxDmabufManager *MANAGER = nullptr;
}

void LinuxDmabufFormatSelection::ensureSorted() {}

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
        tranche.formats.formats.push(
            qMakePair(entry.format, LinuxDmabufModifiers()));
        modifiers =
            &(--tranche.formats.formats.end())
                 ->second; // there is a better way of doing this without using
                           // janky pointer arithmetic, fix later
      } else {
        modifiers = &modIt->second;
      }
    }

    if (entry.modifier == DRM_FORMAT_MOD_INVALID) {
      modifiers->implicit = true;
    } else {
      modifiers->modifiers.push(entry.modifier);
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
} // namespace ns::wayland::buffer::dmabuf
