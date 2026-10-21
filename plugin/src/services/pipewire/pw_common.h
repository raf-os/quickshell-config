#pragma once

#include <qflags.h>
#include <qlist.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <spa/param/audio/raw.h>
#include <spa/pod/pod.h>

#include "ns_services_pipewire_export.h"

namespace ns::services::pipewire {
namespace AudioChannel {
Q_NAMESPACE_EXPORT(NS_SERVICES_PIPEWIRE_EXPORT)
QML_NAMED_ELEMENT(AudioChannel)

enum Enum {
  Unknown                  = SPA_AUDIO_CHANNEL_UNKNOWN,
  NA                       = SPA_AUDIO_CHANNEL_NA,
  Mono                     = SPA_AUDIO_CHANNEL_MONO,
  FrontCenter              = SPA_AUDIO_CHANNEL_FC,
  FrontLeft                = SPA_AUDIO_CHANNEL_FL,
  FrontRight               = SPA_AUDIO_CHANNEL_FR,
  FrontLeftCenter          = SPA_AUDIO_CHANNEL_FLC,
  FrontRightCenter         = SPA_AUDIO_CHANNEL_FRC,
  FrontLeftWide            = SPA_AUDIO_CHANNEL_FLW,
  FrontRightWide           = SPA_AUDIO_CHANNEL_FRW,
  FrontCenterHigh          = SPA_AUDIO_CHANNEL_FCH,
  FrontLeftHigh            = SPA_AUDIO_CHANNEL_FLH,
  FrontRightHigh           = SPA_AUDIO_CHANNEL_FRH,
  LowFrequencyEffects      = SPA_AUDIO_CHANNEL_LFE,
  LowFrequencyEffects2     = SPA_AUDIO_CHANNEL_LFE2,
  LowFrequencyEffectsLeft  = SPA_AUDIO_CHANNEL_LLFE,
  LowFrequencyEffectsRight = SPA_AUDIO_CHANNEL_RLFE,
  SideLeft                 = SPA_AUDIO_CHANNEL_SL,
  SideRight                = SPA_AUDIO_CHANNEL_SR,
  RearCenter               = SPA_AUDIO_CHANNEL_RC,
  RearLeft                 = SPA_AUDIO_CHANNEL_RL,
  RearRight                = SPA_AUDIO_CHANNEL_RR,
  RearLeftCenter           = SPA_AUDIO_CHANNEL_RLC,
  RearRightCenter          = SPA_AUDIO_CHANNEL_RRC,
  TopCenter                = SPA_AUDIO_CHANNEL_TC,
  TopFrontCenter           = SPA_AUDIO_CHANNEL_TFC,
  TopFrontLeft             = SPA_AUDIO_CHANNEL_TFL,
  TopFrontRight            = SPA_AUDIO_CHANNEL_TFR,
  TopFrontLeftCenter       = SPA_AUDIO_CHANNEL_TFLC,
  TopFrontRightCenter      = SPA_AUDIO_CHANNEL_TFRC,
  TopSideLeft              = SPA_AUDIO_CHANNEL_TSL,
  TopSideRight             = SPA_AUDIO_CHANNEL_TSR,
  TopRearCenter            = SPA_AUDIO_CHANNEL_TRC,
  TopRearLeft              = SPA_AUDIO_CHANNEL_TRL,
  TopRearRight             = SPA_AUDIO_CHANNEL_TRR,
  BottomCenter             = SPA_AUDIO_CHANNEL_BC,
  BottomLeftCenter         = SPA_AUDIO_CHANNEL_BLC,
  BottomRightCenter        = SPA_AUDIO_CHANNEL_BRC,
  AuxRangeStart            = SPA_AUDIO_CHANNEL_START_Aux,
  AuxRangeEnd              = SPA_AUDIO_CHANNEL_LAST_Aux,
  CustomRangeStart         = SPA_AUDIO_CHANNEL_START_Custom,
};

Q_ENUM_NS(Enum)
} // namespace AudioChannel

namespace NodeType {
Q_NAMESPACE_EXPORT(NS_SERVICES_PIPEWIRE_EXPORT)
QML_NAMED_ELEMENT(NodeType)

enum Flag : quint8 {
  Untracked      = 0,
  Audio          = 1 << 0,
  Video          = 1 << 1,
  Stream         = 1 << 2,
  Source         = 1 << 3,
  Sink           = 1 << 4,
  AudioSink      = Audio | Sink,
  AudioSource    = Audio | Source,
  AudioDuplex    = Audio | Sink | Source,
  AudioOutStream = Audio | Sink | Stream,
  AudioInStream  = Audio | Source | Stream,
  VideoSource    = Video | Source,
  VideoSink      = Video | Sink
};

Q_ENUM_NS(Flag)
Q_DECLARE_FLAGS(Flags, Flag)
Q_DECLARE_OPERATORS_FOR_FLAGS(Flags)
} // namespace NodeType

struct NS_SERVICES_PIPEWIRE_EXPORT VolumeProps {
  QList<AudioChannel::Enum> channels;
  QList<float>              volumes;
  bool                      muted      = false;
  float                     volumeStep = -1;
};

VolumeProps NS_SERVICES_PIPEWIRE_EXPORT volume_props_from_spa_pod(
    const spa_pod *param);
} // namespace ns::services::pipewire
