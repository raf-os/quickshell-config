#include "mpris_player.h"

#include <qdbusconnection.h>
#include <qloggingcategory.h>
#include <qobject.h>

#include "dbus_mpris.h"
#include "ns_mpris_debug.h"

namespace ns::mpris {
MprisPlayer::MprisPlayer(const QString &service, QObject *parent)
    : QObject(parent), m_service(service) {
  m_player = new org::mpris::MediaPlayer2(
      service, "/org/mpris/MediaPlayer2", QDBusConnection::sessionBus(), this);

  if (!m_player->isValid()) {
    qCWarning(NS_LOG_MPRIS)
        << "Unable to create mpris object for address" << service;
    return;
  }
}
} // namespace ns::mpris
