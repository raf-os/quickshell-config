#include "mpris_player.h"

#include <qdbusconnection.h>
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
  QObject::connect(&DBUS_BINDABLE_MEMBER, &dbus::DBusBindableGroup::getAllDone,
      this, &MprisPlayer::ready);

  m_controls = new MprisPlayerControls(service, this);

  DBUS_BINDABLE_MEMBER.getAll();
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

bool MprisPlayer::isValid() const { return m_player->isValid(); }

MprisPlayerControls *MprisPlayer::controls() { return m_controls; }
} // namespace ns::mpris
