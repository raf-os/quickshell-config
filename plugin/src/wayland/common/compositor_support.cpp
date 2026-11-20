#include "compositor_support.h"

namespace ns::wayland {
CompositorData::CompositorData() {}

CompositorData::~CompositorData() = default;

CompositorSupport CompositorData::current() const { return m_activeCompositor; }
} // namespace ns::wayland
