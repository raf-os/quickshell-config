#pragma once

#include <pipewire/device.h>
#include <qtmetamacros.h>

#include "ns_services_pipewire_export.h"
#include "pw_bindable.h"
#include "pw_spa_hook.h"

namespace ns::services::pipewire {
class NS_SERVICES_PIPEWIRE_EXPORT PwDevice : public PwBindableBase {
  Q_OBJECT

public:
  explicit PwDevice(PwBindableProps props);

  void bindHooks() override;
  void unbindHooks() override;

private slots:
  void onCorePolled();

private:
  PwBindableInterface<pw_device, PW_TYPE_INTERFACE_Device, PW_VERSION_DEVICE>
      m_interface{this};

  SpaHook m_listener;
};
} // namespace ns::services::pipewire
