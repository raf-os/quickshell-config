#pragma once

#include <functional>
#include <optional>

#include <pipewire/device.h>
#include <qhash.h>
#include <qlist.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <spa/pod/builder.h>
#include <spa/pod/pod.h>

#include "ns_services_pipewire_export.h"
#include "pw_bindable.h"
#include "pw_common.h"
#include "pw_spa_hook.h"

namespace ns::services::pipewire {
class NS_SERVICES_PIPEWIRE_EXPORT Device : public PwBindableBase {
  Q_OBJECT

public:
  explicit Device(PwBindableProps props);

  void bindHooks() override;
  void unbindHooks() override;

  void               waitForDevice();
  [[nodiscard]] bool waitingForDevice() const;

  bool                               hasRouteDevice(qint32 device) const;
  std::optional<const VolumeProps *> getVolumeProps(qint32 device);

  static void pw_device_on_info(void *data, const struct pw_device_info *info);
  static void pw_device_on_param(void *data, int seq, uint32_t id,
      uint32_t index, uint32_t next, const struct spa_pod *param);

  bool setVolumes(qint32 device, const QList<float> &volumes);
  bool setMuted(qint32 device, bool muted);

  struct DeviceProps {
    qint32      index;
    VolumeProps volumeProps;
  };

signals:
  void deviceReady();
  void routeVolumesChanged(qint32 routeDevice, const VolumeProps *volumeProps);

private slots:
  void onCorePolled();

private:
  SpaHook m_listener;

  QList<qint32> m_stagingIndexes;
  bool          m_deviceResponded  = false;
  bool          m_waitingForDevice = false;

  QHash<qint32, DeviceProps> m_routeDevices;

  PwBindableInterface<pw_device, PW_TYPE_INTERFACE_Device, PW_VERSION_DEVICE>
      m_interface{this};

  void addDeviceIndexPairs(const spa_pod *param);
  bool setRouteProps(qint32                           routeDevice,
      const std::function<void *(spa_pod_builder *)> &propsCallback);
};
} // namespace ns::services::pipewire
