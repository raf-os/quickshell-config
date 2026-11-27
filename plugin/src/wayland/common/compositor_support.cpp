#include "compositor_support.h"

namespace ns::wayland {
CompositorData::CompositorData()
    : m_activeCompositor(CompositorSupport::Hyprland) {}

CompositorSupport CompositorData::current() const { return m_activeCompositor; }
} // namespace ns::wayland
