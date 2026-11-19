import qs.components

import org.nightshell.Configs

import QtQuick
import QtQuick.Layouts

MouseArea {
  id: root

  required property string icon
  property int size: 16
  property bool flipH: false
  property bool interactable: true
  property color color: Colors.primary

  implicitWidth: size
  implicitHeight: size

  cursorShape: (enabled && interactable) ? Qt.PointingHandCursor : Qt.ArrowCursor

  ShellIcon {
    id: cIcon

    anchors.centerIn: parent
    icon: root.icon
    size: 24
    scale: root.size / size

    colorize: true
    color: root.color
    glow: true

    transform: Scale {
      xScale: root.flipH ? -1 : 1
      origin.x: cIcon.width / 2
    }
  }
}
