#include "gbmdevice.h"

#include <memory>

#include <qloggingcategory.h>
#include <unistd.h>

#include "linuxdmabufmanager.h"

namespace ns::wayland::buffer::dmabuf {
Q_DECLARE_LOGGING_CATEGORY(logNSDmabuf)

GbmDevice::~GbmDevice() {
  qCDebug(logNSDmabuf) << "Destroying GBM device on render node"
                       << this->renderNode.c_str();
  auto fd = gbm_device_get_fd(this->device);
  gbm_device_destroy(this->device);
  close(fd);

  // remove devices from global manager
  auto m = LinuxDmabufManager::getManager();
  m->gbmDevices.removeIf(
      [](const std::weak_ptr<GbmDevice> &d) { return d.expired(); });
}
} // namespace ns::wayland::buffer::dmabuf
