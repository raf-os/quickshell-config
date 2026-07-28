#include "wlbufferrequest.h"

#include <cstdint>

#include <drm_fourcc.h>
#include <qvarlengtharray.h>

namespace ns::wayland::buffer {
void WlBufferRequest::DmaFormat::pushMod(uint64_t mod) {
  if (mod == DRM_FORMAT_MOD_INVALID) {
    this->implicit = true;
  } else {
    this->modifiers.push_back(mod);
  }
}

bool WlBufferRequest::DmaFormat::isModsDefined() const {
  return this->implicit || !this->modifiers.isEmpty();
}

void WlBufferRequest::reset() {
  this->width          = 0;
  this->height         = 0;
  this->shm.formats    = QVarLengthArray<uint32_t, 1>{};
  this->dmabuf.device  = 0;
  this->dmabuf.formats = QVarLengthArray<DmaFormat, 1>{};
}
} // namespace ns::wayland::buffer
