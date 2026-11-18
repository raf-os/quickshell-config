#pragma once

#include <qcontainerfwd.h>
#include <qelapsedtimer.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "dbus_bindable_group.h"
#include "dbus_mpris_player.h"
#include "helpermacros.h"
#include "ns_mpris_export.h"

namespace ns::mpris {
namespace PlaybackState {
Q_NAMESPACE_EXPORT(NS_MPRIS_EXPORT)

enum Enum { Stopped = 0, Playing = 1, Paused = 2 };

Q_ENUM_NS(Enum)
QML_NAMED_ELEMENT(PlaybackState)
}; // namespace PlaybackState

namespace LoopState {
Q_NAMESPACE_EXPORT(NS_MPRIS_EXPORT)

enum Enum { None = 0, Track = 1, Playlist = 2 };

Q_ENUM_NS(Enum)
QML_NAMED_ELEMENT(LoopState)
} // namespace LoopState

class MprisPlayerControls : public QObject {
  Q_OBJECT

  Q_PROPERTY(qint64 position READ position NOTIFY positionChanged)

public:
  using interface_t = DBusMprisMediaPlayerControls;
  using this_t      = MprisPlayerControls;

private:
  AUTO_BINDABLE(MprisPlayerControls, bool, canGoNext)
  AUTO_BINDABLE(MprisPlayerControls, bool, canGoPrevious)
  AUTO_BINDABLE(MprisPlayerControls, bool, canPlay)
  AUTO_BINDABLE(MprisPlayerControls, bool, canSeek)
  AUTO_BINDABLE(MprisPlayerControls, bool, canPause)

  AUTO_BINDABLE(
      MprisPlayerControls, ns::mpris::PlaybackState::Enum, playbackState)
  AUTO_BINDABLE(MprisPlayerControls, ns::mpris::LoopState::Enum, loopState)

  AUTO_BINDABLE(MprisPlayerControls, QString, artUrl)
  AUTO_BINDABLE(MprisPlayerControls, QString, trackTitle)
  AUTO_BINDABLE(MprisPlayerControls, QString, albumName)
  AUTO_BINDABLE(MprisPlayerControls, qint64, trackLength)

private:
  DBUS_BINDABLE_ATTACHED(MprisPlayer)
  DBUS_BINDABLE_PROPERTY(
      MprisPlayerControls, qreal, rate, Rate, {.writable = true})
  DBUS_BINDABLE_PROPERTY(
      MprisPlayerControls, bool, shuffle, Shuffle, {.writable = true})

  DBUS_BINDABLE_PROPERTY(MprisPlayerControls, QVariantMap, metadata, Metadata)
  DBUS_BINDABLE_PROPERTY(
      MprisPlayerControls, qreal, volume, Volume, {.writable = true})
  DBUS_BINDABLE_PROPERTY(MprisPlayerControls, qreal, minimumRate, MinimumRate)
  DBUS_BINDABLE_PROPERTY(MprisPlayerControls, qreal, maximumRate, MaximumRate)

  DBUS_BINDABLE_PROPERTY(
      MprisPlayerControls, bool, canControl, CanControl, {.hasSignal = false})

  DBUS_BINDABLE_PROPERTY_INTERNAL(
      MprisPlayerControls, QByteArray, playbackStatus, PlaybackStatus)
  DBUS_BINDABLE_PROPERTY_INTERNAL(
      MprisPlayerControls, QByteArray, loopStatus, LoopStatus)
  DBUS_BINDABLE_PROPERTY_INTERNAL(
      MprisPlayerControls, qint64, position, Position, {.hasSignal = false})

  DBUS_BINDABLE_PROPERTY_INTERNAL(
      MprisPlayerControls, bool, canGoNext, CanGoNext)
  DBUS_BINDABLE_PROPERTY_INTERNAL(
      MprisPlayerControls, bool, canGoPrevious, CanGoPrevious)
  DBUS_BINDABLE_PROPERTY_INTERNAL(MprisPlayerControls, bool, canPlay, CanPlay)
  DBUS_BINDABLE_PROPERTY_INTERNAL(MprisPlayerControls, bool, canPause, CanPause)
  DBUS_BINDABLE_PROPERTY_INTERNAL(MprisPlayerControls, bool, canSeek, CanSeek)

public:
  explicit MprisPlayerControls(
      const QString &address, QObject *parent = nullptr);

  [[nodiscard]] bool         isValid() const;
  [[nodiscard]] interface_t *interface();

  [[nodiscard]] qint64         position() const;
  [[nodiscard]] constexpr bool isPlaying() const;

signals:
  void positionChanged();

private slots:
  void onMetadataChanged();
  void onProgressTimerTimeout();
  void onPlaybackStateChanged();
  void onPositionChanged();
  void onSeeked(qint64 pos);
  void onRateChanged();

private:
  DBusMprisMediaPlayerControls *m_interface = nullptr;

  qint64 m_pos = 0;

  QTimer m_progressTimer;

  QPropertyNotifier _position_change_notifier;

  friend class MprisPlayer;
};
} // namespace ns::mpris
