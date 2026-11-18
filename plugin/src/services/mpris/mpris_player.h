#pragma once

#include <qcontainerfwd.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "dbus_bindable_group.h"
#include "dbus_mpris.h"
#include "helpermacros.h"
#include "mpris_player_controls.h"

namespace ns::mpris {
class MprisPlayerControls;

class MprisPlayer : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(bool canControl READ canControl NOTIFY canControlChanged)
  Q_PROPERTY(ns::mpris::PlaybackState::Enum playbackState READ playbackState
          NOTIFY playbackStateChanged)
  Q_PROPERTY(QString trackTitle READ trackTitle NOTIFY trackTitleChanged)
  Q_PROPERTY(QString artUrl READ artUrl NOTIFY artUrlChanged)
  Q_PROPERTY(QString albumName READ albumName NOTIFY albumNameChanged)

  AUTO_BINDABLE(MprisPlayer, bool, canReadPosition)
  AUTO_BINDABLE(MprisPlayer, bool, canGoPrevious)
  AUTO_BINDABLE(MprisPlayer, bool, canGoNext)
  AUTO_BINDABLE(MprisPlayer, bool, canPlay)
  AUTO_BINDABLE(MprisPlayer, bool, canPause)
  AUTO_BINDABLE(MprisPlayer, qreal, progress)

public:
  explicit MprisPlayer(const QString &service, QObject *parent = nullptr);

  Q_INVOKABLE void raise();
  Q_INVOKABLE void quit();
  Q_INVOKABLE void fullscreen(bool state);

  Q_INVOKABLE void togglePlay();
  Q_INVOKABLE void goNext();
  Q_INVOKABLE void goPrevious();

  [[nodiscard]] bool                canControl() const;
  [[nodiscard]] PlaybackState::Enum playbackState() const;
  [[nodiscard]] QString             trackTitle() const;
  [[nodiscard]] QString             artUrl() const;
  [[nodiscard]] QString             albumName() const;

  [[nodiscard]] bool                 isValid() const;
  [[nodiscard]] MprisPlayerControls *controls();

signals:
  void ready();
  void canControlChanged();
  void playbackStateChanged();
  void trackTitleChanged();
  void artUrlChanged();
  void albumNameChanged();
  void positionChanged();

private slots:
  void onPositionChanged();
  void onTrackLengthChanged();

private:
  const QString m_service;

  DBusMprisMediaPlayer *m_player   = nullptr;
  MprisPlayerControls  *m_controls = nullptr;

  QMetaObject::Connection m_positionConnection;

  DBUS_BINDABLE_ATTACHED(MprisPlayer)
  DBUS_BINDABLE_PROPERTY(MprisPlayer, bool, canQuit, CanQuit)
  DBUS_BINDABLE_PROPERTY(
      MprisPlayer, bool, fullscreen, Fullscreen, {.writable = true})
  DBUS_BINDABLE_PROPERTY(MprisPlayer, bool, canSetFullscreen, CanSetFullscreen)
  DBUS_BINDABLE_PROPERTY(MprisPlayer, bool, canRaise, CanRaise)
  DBUS_BINDABLE_PROPERTY(MprisPlayer, bool, hasTrackList, HasTrackList)
  DBUS_BINDABLE_PROPERTY(MprisPlayer, QString, identity, Identity)
  DBUS_BINDABLE_PROPERTY(MprisPlayer, QString, desktopEntry, DesktopEntry)
  DBUS_BINDABLE_PROPERTY(
      MprisPlayer, QStringList, supportedUriSchemes, SupportedUriSchemes)
  DBUS_BINDABLE_PROPERTY(
      MprisPlayer, QStringList, supportedMimeTypes, SupportedMimeTypes)
};
} // namespace ns::mpris
