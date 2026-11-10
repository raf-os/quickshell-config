import "popouts"

import qs.services
import qs.components

import org.nightshell.Configs

import QtQuick

BaseStatus {
  id: root
  name: "audio"

  readonly property bool isMuted: AudioService.defaultSinkMuted
  tooltipText: "Audio Settings"

  popoutComponent: AudioPopout {}

  ShellIcon {
    id: icon
    size: 24
    colorize: true
    glow: true
    color: root.isActive ? Colors.accent : (root.isMuted ? Colors.primary : Colors.secondary)
    anchors.fill: parent
    icon: AudioService.sinkIcon
  }
}
