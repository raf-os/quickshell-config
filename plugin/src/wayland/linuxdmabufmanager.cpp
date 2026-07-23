#include "linuxdmabufmanager.h"

#include <memory>

#include <fcntl.h>
#include <gbm.h>
#include <qlogging.h>
#include <qloggingcategory.h>
#include <qscopeguard.h>
#include <qwaylandclientextension.h>
#include <unistd.h>
#include <xf86drm.h>

#include "manager_p.h"

namespace ns::wayland::buffer::dmabuf {
Q_DECLARE_LOGGING_CATEGORY(logNSDmabuf) // from dmabuf.cpp

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
