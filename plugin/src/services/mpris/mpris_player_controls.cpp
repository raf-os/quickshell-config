#include "mpris_player_controls.h"

#include <qdbusconnection.h>
#include <qloggingcategory.h>
#include <qmetaobject.h>
#include <qobject.h>

#include "dbus_bindable_group.h"
#include "dbus_mpris_player.h"
#include "ns_mpris_debug.h"

namespace ns::mpris {
MprisPlayerControls::MprisPlayerControls(
    const QString &address, QObject *parent)
    : QObject(parent) {
  m_interface = new DBusMprisMediaPlayerControls(
      address, "/org/mpris/MediaPlayer2", QDBusConnection::sessionBus(), this);

  if (!m_interface->isValid()) {
    qCWarning(NS_LOG_MPRIS)
        << "Unable to create MprisPlayerControls for" << address;
    return;
  }

  DBUS_BINDABLE_MEMBER.setInterface(m_interface);

  QObject::connect(this, &MprisPlayerControls::metadataChanged, this,
      &MprisPlayerControls::onMetadataChanged);

  b_canGoNext.setBinding(
      [this] { return bi_canGoNext.value() && b_canControl.value(); });
  b_canGoPrevious.setBinding(
      [this] { return bi_canGoPrevious.value() && b_canControl.value(); });
  b_canPlay.setBinding(
      [this] { return bi_canPlay.value() && b_canControl.value(); });
  b_canSeek.setBinding(
      [this] { return bi_canSeek.value() && b_canControl.value(); });
  b_canPause.setBinding(
      [this] { return bi_canPause.value() && b_canControl.value(); });

  b_playbackState.setBinding([this] {
    auto pbStr = bi_playbackStatus.value().constData();
    bool ok    = false;
    auto val =
        QMetaEnum::fromType<PlaybackState::Enum>().keyToValue(pbStr, &ok);

    if (!ok) return PlaybackState::Stopped;
    return static_cast<PlaybackState::Enum>(val);
  });

  b_loopState.setBinding([this] {
    auto lpStr = bi_loopStatus.value().constData();
    bool ok    = false;
    auto val   = QMetaEnum::fromType<LoopState::Enum>().keyToValue(lpStr, &ok);

    if (!ok) return LoopState::None;
    return static_cast<LoopState::Enum>(val);
  });

  b_artUrl.setBinding(
      [this] { return b_metadata.value().value("mpris:artUrl").toString(); });

  b_trackLength.setBinding([this] {
    auto v = b_metadata.value().value("mpris:length");
    if (v.isValid() && v.canConvert<qint64>()) {
      return v.value<qint64>();
    } else {
      return static_cast<qint64>(-1);
    }
  });

  b_trackTitle.setBinding(
      [this] { return b_metadata.value().value("xesam:title").toString(); });
}

void MprisPlayerControls::onMetadataChanged() {
  const auto mData = b_metadata.value();
}
} // namespace ns::mpris
