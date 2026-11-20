#pragma once

#include <cstdint>

namespace ns::wayland {
enum class CompositorSupport : uint32_t { None = 0, Hyprland = 1 << 0 };

class CompositorData {
public:
  explicit CompositorData();
  virtual ~CompositorData();

  [[nodiscard]] CompositorSupport current() const;

private:
  CompositorSupport m_activeCompositor;
};
} // namespace ns::wayland
