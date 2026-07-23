#pragma once

#include "manager.h"

namespace ns::wayland::buffer {
class WlBufferManagerPrivate {
public:
  explicit WlBufferManagerPrivate(WlBufferManager *manager);

  void dmabufReady();

  WlBufferManager *manager;
  // dmabuf

  bool dmabufFormatsReady  = false;
  bool renderFormatsReady  = false;
  bool renderFormatsFailed = false;
};
} // namespace ns::wayland::buffer
