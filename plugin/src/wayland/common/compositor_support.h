#pragma once

#include <cstdint>

#include <QObject>

namespace ns::wayland {
namespace Compositor {
Q_NAMESPACE

enum class Enum : uint32_t { // NOLINT
  None     = 0,
  Hyprland = 1 << 0
};

Q_ENUM_NS(Enum)
} // namespace Compositor

using CompositorSupport = Compositor::Enum;

class CompositorData {
public:
  explicit CompositorData();
  virtual ~CompositorData();

  [[nodiscard]] CompositorSupport current() const;

private:
  CompositorSupport m_activeCompositor;
};
} // namespace ns::wayland
