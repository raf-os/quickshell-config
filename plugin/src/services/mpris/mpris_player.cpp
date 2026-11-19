#include "mpris_player.h"

#include <algorithm>

#include <qdbusconnection.h>
#include <qhashfunctions.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qqmlinfo.h>

#include "dbus_bindable_group.h"
#include "dbus_mpris.h"
#include "mpris_player_controls.h"
#include "ns_mpris_debug.h"

namespace ns::mpris {
MprisPlayer::MprisPlayer(const QString &service, QObject *parent)
    : QObject(parent), m_service(service) {
  m_player = new DBusMprisMediaPlayer(
      service, "/org/mpris/MediaPlayer2", QDBusConnection::sessionBus(), this);

  if (!m_player->isValid()) {
    qCWarning(NS_LOG_MPRIS)
        << "Unable to create mpris object for address" << service;
    return;
  }

  DBUS_BINDABLE_MEMBER.setInterface(m_player);
  DBUS_BINDABLE_MEMBER.getAll();
  QObject::connect(&DBUS_BINDABLE_MEMBER, &dbus::DBusBindableGroup::getAllDone,
      this, &MprisPlayer::ready);

  m_controls = new MprisPlayerControls(service, this);

  if (!m_controls->isValid()) {
  } else {
    b_canGoNext.setBinding(
        [this] { return m_controls->bindable_canGoNext().value(); });
    b_canGoPrevious.setBinding(
        [this] { return m_controls->bindable_canGoPrevious().value(); });
    b_canPlay.setBinding(
        [this] { return m_controls->bindable_canPlay().value(); });
    b_canPause.setBinding(
        [this] { return m_controls->bindable_canPause().value(); });

    b_canReadPosition.setBinding([this] {
      return m_controls->bindable_trackLength().value() !=
             static_cast<qint64>(-1);
    });

    QObject::connect(m_controls, &MprisPlayerControls::playbackStateChanged,
        this, &MprisPlayer::playbackStateChanged);
    QObject::connect(m_controls, &MprisPlayerControls::loopStateChanged, this,
        &MprisPlayer::loopStateChanged);
    QObject::connect(m_controls, &MprisPlayerControls::canControlChanged, this,
        &MprisPlayer::canControlChanged);
    QObject::connect(m_controls, &MprisPlayerControls::trackTitleChanged, this,
        &MprisPlayer::trackTitleChanged);
    QObject::connect(m_controls, &MprisPlayerControls::trackLengthChanged, this,
        &MprisPlayer::onTrackLengthChanged);
    QObject::connect(m_controls, &MprisPlayerControls::artUrlChanged, this,
        &MprisPlayer::artUrlChanged);
    QObject::connect(m_controls, &MprisPlayerControls::albumNameChanged, this,
        &MprisPlayer::albumNameChanged);
    QObject::connect(m_controls, &MprisPlayerControls::shuffleChanged, this,
        &MprisPlayer::shuffleChanged);

    onTrackLengthChanged();
  }
}

void MprisPlayer::onTrackLengthChanged() {
  if (m_controls->bindable_trackLength().value() == static_cast<qint64>(-1)) {
    if (m_positionConnection) {
      QObject::disconnect(m_positionConnection);
    }
    b_progress = 0;
    return;
  }

  if (!m_positionConnection) {
    m_positionConnection =
        QObject::connect(m_controls, &MprisPlayerControls::positionChanged,
            this, &MprisPlayer::onPositionChanged);
  }

  onPositionChanged();
}

void MprisPlayer::onPositionChanged() {
  if (!b_canReadPosition.value()) b_progress = 0;

  qreal len = m_controls->bindable_trackLength().value();
  qreal pos = m_controls->position();

  b_progress = std::min<qreal>(1, pos / len);
}

void MprisPlayer::raise() {
  if (!b_canRaise.value()) {
    qmlWarning(this) << "Error raising" << b_identity.value()
                     << ": interface does not support raising.";
    return;
  }
  m_player->Raise();
}

void MprisPlayer::quit() {
  if (!b_canQuit.value()) {
    qmlWarning(this) << "Error quitting" << b_identity.value()
                     << ": interface does not support quitting.";
    return;
  }
  m_player->Quit();
}

void MprisPlayer::fullscreen(bool state) {
  if (!b_canSetFullscreen.value()) {
    qmlWarning(this)
        << "Error quitting" << b_identity.value()
        << ": interface does not support changing fullscreen state.";
    return;
  }

  Fullscreen.set(state);
}

void MprisPlayer::toggleShuffle() {
  if (!m_controls) return;

  if (!m_controls->bindableCanControl().value()) {
    qmlWarning(this)
        << "Unable to set shuffle state: mpris object cannot be controlled.";
    return;
  }

  const auto currentValue = m_controls->bindableShuffle().value();
  m_controls->Shuffle.set(!currentValue);
}

void MprisPlayer::togglePlay() {
  if (!m_controls) {
    qmlWarning(this)
        << "Cannot toggle play as object does not have attached controls.";
    return;
  }

  const auto canToggle = m_controls->bindable_canPlay().value() &&
                         m_controls->bindable_canPause().value();
  if (!canToggle) {
    qmlWarning(this) << "Mpris object does not have play/pause capabilities.";
    return;
  }

  m_controls->interface()->PlayPause();
}

void MprisPlayer::goNext() {
  if (!m_controls) return;

  if (!m_controls->bindable_canGoNext().value()) {
    qmlWarning(this) << "Mpris object does not have go next capabilities.";
    return;
  }

  m_controls->interface()->Next();
}

void MprisPlayer::goPrevious() {
  if (!m_controls) return;

  if (!m_controls->bindable_canGoPrevious().value()) {
    qmlWarning(this) << "Mpris object does not have go previous capabilities.";
    return;
  }

  m_controls->interface()->Previous();
}

bool MprisPlayer::canControl() const {
  if (!m_controls) return false;
  return m_controls->bindableCanControl().value();
}

PlaybackState::Enum MprisPlayer::playbackState() const {
  if (!m_controls) return PlaybackState::Stopped;
  return m_controls->bindable_playbackState().value();
}

LoopState::Enum MprisPlayer::loopState() const {
  if (!m_controls) return LoopState::None;
  return m_controls->bindable_loopState().value();
}

QString MprisPlayer::trackTitle() const {
  const auto t = m_controls->bindable_trackTitle().value();
  if (t.isEmpty()) return QStringLiteral("Unknown Title");
  else return t;
}

QString MprisPlayer::artUrl() const {
  if (!m_controls) return {};
  return m_controls->bindable_artUrl().value();
}

QString MprisPlayer::albumName() const {
  if (!m_controls) return {};
  return m_controls->bindable_albumName().value();
}

bool MprisPlayer::shuffle() const {
  if (!m_controls) return {};
  return m_controls->bindableShuffle().value();
}

bool MprisPlayer::isValid() const { return m_player->isValid(); }

MprisPlayerControls *MprisPlayer::controls() { return m_controls; }
} // namespace ns::mpris
