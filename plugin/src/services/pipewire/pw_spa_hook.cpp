#include "pw_spa_hook.h"

namespace ns::services::pipewire {
SpaHook::SpaHook() { spa_zero(this->hook); }

void SpaHook::remove() {
  spa_hook_remove(&this->hook);
  spa_zero(this->hook);
}
} // namespace ns::services::pipewire
