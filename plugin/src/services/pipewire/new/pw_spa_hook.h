#pragma once

#include <spa/utils/hook.h>

namespace ns::pipewire {
class SpaHook {
public:
  explicit SpaHook();

  void     remove();
  spa_hook hook;
};
} // namespace ns::pipewire
