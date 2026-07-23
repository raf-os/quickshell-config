#include "dmabuf.h"

#include <gbm.h>
#include <qdebug.h>
#include <qloggingcategory.h>
#include <unistd.h>

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

GbmDevice::~GbmDevice() {
  qCDebug(logNSDmabuf) << "Destroying GBM device on render node"
                       << this->renderNode.c_str();
  auto fd = gbm_device_get_fd(this->device);
  gbm_device_destroy(this->device);
  close(fd);

  // remove devices from global manager
}
} // namespace ns::wayland::buffer::dmabuf
