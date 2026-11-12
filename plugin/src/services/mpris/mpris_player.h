#pragma once

#include <qobject.h>
#include <qtmetamacros.h>

#include "dbus_mpris.h"
#include "helpermacros.h"

namespace ns::mpris {
class MprisPlayer : public QObject {
  Q_OBJECT

  AUTO_BINDABLE(MprisPlayer, bool, CanQuit)
  AUTO_BINDABLE(MprisPlayer, bool, CanSetFullscreen)
  AUTO_BINDABLE(MprisPlayer, bool, CanRaise)
  AUTO_BINDABLE(MprisPlayer, bool, HasTrackList)

public:
  explicit MprisPlayer(const QString &service, QObject *parent = nullptr);

private:
  const QString m_service;

  org::mpris::MediaPlayer2 *m_player = nullptr;
};
} // namespace ns::mpris
