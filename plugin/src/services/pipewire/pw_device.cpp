#include "pw_device.h"

#include <cstdint>
#include <optional>
#include <utility>

#include <pipewire/device.h>
#include <qlist.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qtypes.h>
#include <spa/param/param.h>
#include <spa/param/props.h>
#include <spa/param/route.h>
#include <spa/pod/builder.h>
#include <spa/pod/parser.h>
#include <spa/pod/pod.h>
#include <spa/pod/vararg.h>
#include <spa/utils/type.h>

#include "ns_services_pipewire_debug.h"
#include "pw_bindable.h"
#include "pw_common.h"
#include "pwcore.h"
#include "pwregistry.h"

static const struct pw_device_events NS_PW_DEVICE_EVENTS = {
    .version = PW_VERSION_DEVICE,
    .info    = &ns::services::pipewire::Device::pw_device_on_info,
    .param   = &ns::services::pipewire::Device::pw_device_on_param};

namespace ns::services::pipewire {
Device::Device(PwBindableProps props) : PwBindableBase(std::move(props)) {}

void Device::bindHooks() {
  pw_device_add_listener(
      m_interface.proxy(), &m_listener.hook, &NS_PW_DEVICE_EVENTS, this);
  auto core = m_registry->core();
  QObject::connect(core, &PwCore::polled, this, &Device::onCorePolled);
}
void Device::unbindHooks() {
  QObject::disconnect(
      m_registry->core(), &PwCore::polled, this, &Device::onCorePolled);
  m_listener.remove();
  m_stagingIndexes.clear();
  m_routeDevices.clear();
  m_waitingForDevice = false;
}

void Device::onCorePolled() {
  if (!m_stagingIndexes.isEmpty()) {
    m_routeDevices.removeIf(
        [this](const std::pair<qint32, DeviceProps> &entry) {
          if (!m_stagingIndexes.contains(entry.first)) {
            return true;
          }
          return false;
        });
  }
}

void Device::pw_device_on_info(void *data, const struct pw_device_info *info) {
  auto self = static_cast<Device *>(data);

  if ((info->change_mask & PW_DEVICE_CHANGE_MASK_PARAMS) ==
      PW_DEVICE_CHANGE_MASK_PARAMS)
  {
    for (quint32 i = 0; i < info->n_params; i++) {
      auto &param = info->params[i];

      if (param.id == SPA_PARAM_Route) {
        if ((param.flags & SPA_PARAM_INFO_READWRITE) ==
            SPA_PARAM_INFO_READWRITE)
        {
          self->m_stagingIndexes.clear();
          self->m_deviceResponded = false;
          pw_device_enum_params(
              self->m_interface.proxy(), 0, param.id, 0, UINT32_MAX, nullptr);
        } else {
          qCWarning(NS_LOG_PIPEWIRE)
              << "Received route params without read/write permissions.";
        }

        break;
      }
    }
  }
}

void Device::pw_device_on_param(void *data, int /*seq*/, uint32_t id,
    uint32_t /*index*/, uint32_t /*next*/, const struct spa_pod  *param) {
  auto self = static_cast<Device *>(data);

  if (id == SPA_PARAM_Route) {
    if (!self->m_deviceResponded) {
      self->m_deviceResponded = true;
      if (self->m_waitingForDevice) {
        self->m_waitingForDevice = false;
        emit self->deviceReady();
      }
    }

    self->addDeviceIndexPairs(param);
  }
}

void Device::addDeviceIndexPairs(const spa_pod *param) {
  auto parser = spa_pod_parser();
  spa_pod_parser_pod(&parser, param);

  qint32 device = 0;
  qint32 index  = 0;

  const spa_pod *props = nullptr;

  quint32 id = SPA_PARAM_Route;
  spa_pod_parser_get_object(&parser, SPA_TYPE_OBJECT_ParamRoute, &id,
      SPA_PARAM_ROUTE_device, SPA_POD_Int(&device), SPA_PARAM_ROUTE_index,
      SPA_POD_Int(&index), SPA_PARAM_ROUTE_props, SPA_POD_PodObject(&props));

  auto volumeProps = volume_props_from_spa_pod(props);

  m_stagingIndexes.append(device);
  auto it = m_routeDevices.insert(
      device, {.index = index, .volumeProps = std::move(volumeProps)});

  emit routeVolumesChanged(device, &it.value().volumeProps);
}

bool Device::setRouteProps(qint32                   routeDevice,
    const std::function<void *(spa_pod_builder *)> &propsCallback) {
  if (!m_interface.proxy()) {
    qCCritical(NS_LOG_PIPEWIRE)
        << "Attempted changing device route props for unbound device:" << this;
    return false;
  }

  if (!m_routeDevices.contains(routeDevice)) {
    qCCritical(NS_LOG_PIPEWIRE)
        << this << ": unable to set route props for invalid route device"
        << routeDevice;
    return false;
  }

  auto routeIdx = m_routeDevices.value(routeDevice).index;

  quint8 buff[1024];
  auto   builder = SPA_POD_BUILDER_INIT(&buff, sizeof(buff));

  auto *props = propsCallback(&builder);

  // clang-format off
  auto *route = spa_pod_builder_add_object(&builder,
    SPA_TYPE_OBJECT_ParamRoute, SPA_PARAM_Route,
    SPA_PARAM_ROUTE_device, SPA_POD_Int(routeDevice),
    SPA_PARAM_ROUTE_index, SPA_POD_Int(routeIdx),
    SPA_PARAM_ROUTE_props, SPA_POD_PodObject(props),
    SPA_PARAM_ROUTE_save, SPA_POD_Bool(true)
  );
  // clang-format on

  pw_device_set_param(
      m_interface.proxy(), SPA_PARAM_Route, 0, static_cast<spa_pod *>(route));
  return true;
}

bool Device::setVolumes(qint32 device, const QList<float> &volumes) {
  auto cb = [this, device, &volumes](spa_pod_builder *builder) {
    auto cubedVolumes = QList<float>();
    cubedVolumes.reserve(volumes.size());
    for (auto &volume : volumes) {
      cubedVolumes.append(volume * volume * volume);
    }

    auto *props = spa_pod_builder_add_object(builder, SPA_TYPE_OBJECT_Props,
        SPA_PARAM_Props, SPA_PROP_channelVolumes,
        SPA_POD_Array(sizeof(float), SPA_TYPE_Float, cubedVolumes.size(),
            cubedVolumes.data()));

    qCInfo(NS_LOG_PIPEWIRE) << "Changing volumes of" << this
                            << "on route device" << device << "to" << volumes;

    return props;
  };

  return setRouteProps(device, cb);
}

bool Device::setMuted(qint32 device, bool muted) {
  auto cb = [this, device, muted](spa_pod_builder *builder) {
    auto *props = spa_pod_builder_add_object(builder, SPA_TYPE_OBJECT_Props,
        SPA_PARAM_Props, SPA_PROP_mute, SPA_POD_Bool(muted));

    qCInfo(NS_LOG_PIPEWIRE) << "Changing muted state of" << this
                            << "on route device" << device << "to" << muted;

    return props;
  };

  return setRouteProps(device, cb);
}

void Device::waitForDevice() { m_waitingForDevice = true; }
bool Device::waitingForDevice() const { return m_waitingForDevice; }

bool Device::hasRouteDevice(qint32 device) const {
  return m_routeDevices.contains(device);
}

std::optional<const VolumeProps *> Device::getVolumeProps(qint32 device) {
  auto it = m_routeDevices.find(device);
  if (it == m_routeDevices.end()) return {};
  return &it.value().volumeProps;
}
} // namespace ns::services::pipewire
