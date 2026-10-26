#pragma once

#include <cstdint>
#include <optional>

#include <pipewire/device.h>
#include <qhash.h>
#include <qlist.h>
#include <qtmetamacros.h>
#include <spa/pod/builder.h>
#include <spa/pod/pod.h>

#include "pw_common.h"
#include "pw_spa_hook.h"
#include "pw_trackable.h"

namespace ns::pipewire {
class Device : public TrackedObject<pw_device> {
  Q_OBJECT

public:
  void bindHooks() override;
  void unbindHooks() override;

  bool setVolumes(qint32 device, const QList<float> &volumes);
  bool setMuted(qint32 device, bool muted);

  void               waitForDevice();
  [[nodiscard]] bool waitingForDevice() const;

  bool                               hasRouteDevice(qint32 device) const;
  std::optional<const VolumeProps *> getVolumeProps(qint32 device);

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
  SpaHook m_hook;

  QList<qint32> m_stagingIndexes;
  bool          m_deviceResponded  = false;
  bool          m_waitingForDevice = false;

  QHash<qint32, DeviceProps> m_routeDevices;

  static const struct pw_device_events DEVICE_EVENTS;
  static void on_info(void *data, const struct pw_device_info *info);
  static void on_param(void *data, int seq, uint32_t id, uint32_t index,
      uint32_t next, const struct spa_pod *param);

  void addDeviceIndexPairs(const struct spa_pod *param);
  bool setRouteProps(qint32                           routeDevice,
      const std::function<void *(spa_pod_builder *)> &propsCallback);
};

using TrackableDevice =
    TrackableObject<Device, PW_TYPE_INTERFACE_Device, PW_VERSION_DEVICE>;
} // namespace ns::pipewire
