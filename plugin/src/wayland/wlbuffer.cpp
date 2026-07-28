#include "wlbuffer.h"

#include <qtenvironmentvariables.h>

#include "manager.h"
#include "wlbufferrequest.h"

namespace ns::wayland::buffer {
WlBuffer *WlBufferSwapchain::createBackBuffer(const WlBufferRequest &request,
                                              bool *newBuffer) {
  static const bool noReuse = qEnvironmentVariableIsSet("QS_NO_BUFFER_REUSE");
  auto &buffer = this->presentSecondBuffer ? this->buffer1 : this->buffer2;

  if (!buffer || !buffer->isCompatible(request) || noReuse) {
    // buffer.reset(WlBufferManager::instance()->createBuffer(request));
    if (newBuffer) *newBuffer = true;
  }

  return buffer.get();
}
} // namespace ns::wayland::buffer
