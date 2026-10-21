#include "pw_common.h"

#include <cmath>

#include <spa/param/props.h>
#include <spa/pod/iter.h>
#include <spa/pod/pod.h>

namespace ns::services::pipewire {
VolumeProps volume_props_from_spa_pod(const spa_pod *param) {
  VolumeProps props;

  const auto *volumesProp =
      spa_pod_find_prop(param, nullptr, SPA_PROP_channelVolumes);
  const auto *channelsProp =
      spa_pod_find_prop(param, nullptr, SPA_PROP_channelMap);
  const auto *muteProp = spa_pod_find_prop(param, nullptr, SPA_PROP_mute);
  const auto *volumeStepProp =
      spa_pod_find_prop(param, nullptr, SPA_PROP_volumeStep);

  if (volumesProp) {
    const auto *volumes =
        reinterpret_cast<const spa_pod_array *>(&volumesProp->value);
    spa_pod *iter = nullptr;
    SPA_POD_ARRAY_FOREACH(volumes, iter) {
      auto linear = *reinterpret_cast<float *>(iter);
      auto visual = std::cbrt(linear);
      props.volumes.push_back(visual);
    }
  }

  if (channelsProp) {
    const auto *channels =
        reinterpret_cast<const spa_pod_array *>(&channelsProp->value);
    spa_pod *iter = nullptr;
    SPA_POD_ARRAY_FOREACH(channels, iter) {
      props.channels.push_back(*reinterpret_cast<AudioChannel::Enum *>(iter));
    }
  }

  if (props.channels.empty()) {
    using C = AudioChannel::Enum;
    switch (props.volumes.length()) {
    case 1: props.channels = {C::Mono}; break;
    case 2: props.channels = {C::FrontLeft, C::FrontRight}; break;
    case 3:
      props.channels = {C::FrontLeft, C::FrontRight, C::LowFrequencyEffects};
      break;
    case 4:
      props.channels = {C::FrontLeft, C::FrontRight, C::RearLeft, C::RearRight};
      break;
    case 5:
      props.channels = {C::FrontLeft, C::FrontRight, C::FrontCenter,
          C::SideLeft, C::SideRight};
      break;
    case 6:
      props.channels = {C::FrontLeft, C::FrontRight, C::FrontCenter,
          C::LowFrequencyEffects, C::SideLeft, C::SideRight};
      break;
    case 7:
      props.channels = {C::FrontLeft, C::FrontRight, C::FrontCenter,
          C::RearLeft, C::RearRight, C::SideLeft, C::SideRight};
      break;
    case 8:
      props.channels = {C::FrontLeft, C::FrontRight, C::FrontCenter,
          C::LowFrequencyEffects, C::RearLeft, C::RearRight, C::SideLeft,
          C::SideRight};
      break;
    default: break;
    }
  }

  if (muteProp) {
    spa_pod_get_bool(&muteProp->value, &props.muted);
  }

  if (volumeStepProp) {
    spa_pod_get_float(&volumeStepProp->value, &props.volumeStep);
  } else {
    props.volumeStep = -1;
  }

  return props;
}
} // namespace ns::services::pipewire
