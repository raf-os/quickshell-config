#pragma once

#include <spa/utils/hook.h>

#include "ns_services_pipewire_export.h"

namespace ns::services::pipewire {
class NS_SERVICES_PIPEWIRE_EXPORT SpaHook {
public:
  explicit SpaHook();

  void     remove();
  spa_hook hook;
};
} // namespace ns::services::pipewire
