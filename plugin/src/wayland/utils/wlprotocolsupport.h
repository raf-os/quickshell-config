#pragma once

#include <bitset>
#include <cstdint>

namespace ns::wayland::utils {
class WlProtocolSupport {
public:
  enum Protocols : uint64_t {
    NO_PROTOCOL  = 0,
    SESSION_LOCK = 1 << 0,
    IDLE_NOTIFY  = 1 << 1
  };

  explicit WlProtocolSupport();

private:
  std::bitset<64> m_values;
};
} // namespace ns::wayland::utils
