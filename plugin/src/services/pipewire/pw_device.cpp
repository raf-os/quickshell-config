#include "pw_device.h"

#include <cstdint>
#include <utility>

#include <pipewire/device.h>
#include <qobject.h>
#include <spa/pod/pod.h>

#include "pw_bindable.h"
#include "pwcore.h"
#include "pwregistry.h"

static void pw_device_on_info(void *data, const struct pw_device_info *info) {}
static void pw_device_on_param(void *data, int seq, uint32_t id, uint32_t index,
    uint32_t next, const struct spa_pod *param) {}

static const struct pw_device_events NS_PW_DEVICE_EVENTS = {
    .version = PW_VERSION_DEVICE,
    .info    = pw_device_on_info,
    .param   = pw_device_on_param};

namespace ns::services::pipewire {
PwDevice::PwDevice(PwBindableProps props) : PwBindableBase(std::move(props)) {}

void PwDevice::bindHooks() {
  pw_device_add_listener(
      m_interface.proxy(), &m_listener.hook, &NS_PW_DEVICE_EVENTS, this);
  auto core = m_registry->core();
  QObject::connect(core, &PwCore::polled, this, &PwDevice::onCorePolled);
}
void PwDevice::unbindHooks() {
  m_listener.remove(); //
}

void PwDevice::onCorePolled() {}
} // namespace ns::services::pipewire
