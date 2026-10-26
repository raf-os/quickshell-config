#include "pw_spa_hook.h"

#include <spa/utils/defs.h>
#include <spa/utils/hook.h>

namespace ns::pipewire {
SpaHook::SpaHook() { spa_zero(hook); }

void SpaHook::remove() {
  spa_hook_remove(&hook);
  spa_zero(hook);
}
} // namespace ns::pipewire
