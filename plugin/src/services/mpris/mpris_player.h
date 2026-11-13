#pragma once

#include <qcontainerfwd.h>
#include <qobject.h>
#include <qproperty.h>
#include <qtmetamacros.h>

#include "dbus_bindable_group.h"
#include "dbus_mpris.h"

namespace ns::mpris {
class MprisPlayer : public QObject {
  Q_OBJECT

private:
  DBUS_BINDABLE_ATTACHED
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

public:
  explicit MprisPlayer(const QString &service, QObject *parent = nullptr);

private:
  const QString m_service;

  org::mpris::MediaPlayer2 *m_player = nullptr;
};
} // namespace ns::mpris
