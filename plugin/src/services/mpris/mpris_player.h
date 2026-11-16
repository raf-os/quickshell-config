#pragma once

#include <qcontainerfwd.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

#include "dbus_bindable_group.h"
#include "dbus_mpris.h"

namespace ns::mpris {
class MprisPlayerControls;

class MprisPlayer : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

public:
  explicit MprisPlayer(const QString &service, QObject *parent = nullptr);

  Q_INVOKABLE void raise();
  Q_INVOKABLE void quit();
  Q_INVOKABLE void fullscreen(bool state);

  [[nodiscard]] bool                 isValid() const;
  [[nodiscard]] MprisPlayerControls *controls();

signals:
  void ready();

private:
  const QString m_service;

  DBusMprisMediaPlayer *m_player   = nullptr;
  MprisPlayerControls  *m_controls = nullptr;

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
