#include "mpris_player_controls.h"

#include <algorithm>

#include <qdbusconnection.h>
#include <qloggingcategory.h>
#include <qmetaobject.h>
#include <qobject.h>
#include <qtimer.h>
#include <qtypes.h>

#include "dbus_bindable_group.h"
#include "dbus_mpris_player.h"
#include "ns_mpris_debug.h"

namespace ns::mpris {
namespace {
constexpr int PROGRESS_UPDATE_RESOLUTION_MSEC = 250;
}

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

  b_albumName.setBinding(
      [this] { return b_metadata.value().value("xesam:album").toString(); });

  _position_change_notifier = bi_position.addNotifier(
      dbus::FunctorCallback<&this_t::onPositionChanged>(this));

  m_progressTimer.setInterval(PROGRESS_UPDATE_RESOLUTION_MSEC);
  m_progressTimer.setSingleShot(true);
  QObject::connect(&m_progressTimer, &QTimer::timeout, this,
      &MprisPlayerControls::onProgressTimerTimeout);

  QObject::connect(this, &MprisPlayerControls::playbackStateChanged, this,
      &MprisPlayerControls::onPlaybackStateChanged);
  QObject::connect(this, &MprisPlayerControls::rateChanged, this,
      &MprisPlayerControls::onRateChanged);

  QObject::connect(m_interface, &interface_t::Seeked, this, &this_t::onSeeked);

  DBUS_BINDABLE_MEMBER.getAll();
}

void MprisPlayerControls::onProgressTimerTimeout() {
  Position.get();

  m_progressTimer.start();
}

void MprisPlayerControls::onPositionChanged() {
  m_progressTimer.stop();

  m_pos = bi_position.value();
  emit positionChanged();

  if (isPlaying()) m_progressTimer.start();
}

void MprisPlayerControls::onRateChanged() {
  const qreal interval = PROGRESS_UPDATE_RESOLUTION_MSEC;
  const auto  rate     = std::max<qreal>(0.1, b_rate.value());

  m_progressTimer.setInterval(
      std::max<int>(100, static_cast<int>(interval * (1 / rate))));

  Position.get();
}

void MprisPlayerControls::onSeeked(qint64 pos) { bi_position = pos; }

void MprisPlayerControls::onPlaybackStateChanged() {
  Position.get();
  if (isPlaying()) {
    m_progressTimer.start();
  } else {
    m_progressTimer.stop();
  }
}

void MprisPlayerControls::onMetadataChanged() {
  const auto mData = b_metadata.value();
}

bool MprisPlayerControls::isValid() const { return m_interface->isValid(); }

qint64 MprisPlayerControls::position() const { return m_pos; }

constexpr bool MprisPlayerControls::isPlaying() const {
  return b_playbackState.value() == PlaybackState::Playing;
}

MprisPlayerControls::interface_t *MprisPlayerControls::interface() {
  return m_interface;
}
} // namespace ns::mpris
