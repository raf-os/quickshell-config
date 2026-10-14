import qs.components

import org.nightshell.Components
import org.nightshell.Configs
import org.nightshell.Utils

import QtQuick
import QtQuick.Effects

MouseArea {
  id: root

  required property bool isActive

  property color bgCol: isActive ? Colors.secondary : Colors.primaryContent
  property color fgCol: isActive ? Colors.secondaryContent : Colors.primary
  readonly property int animDuration: 300

  implicitWidth: height + Styles.padding_md * 2
  cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

  Behavior on bgCol {
    CAnim {
      duration: root.animDuration
    }
  }

  SimpleGlowEffect {
    source: bgRect
    anchors.fill: bgRect
    shadowColor: root.isActive ? root.bgCol : Colors.primary
  }

  ChamferRect {
    id: bgRect
    anchors.fill: parent
    color: root.bgCol
    borderWidth: 1
    borderColor: root.isActive ? root.bgCol : Colors.primary
    bottomLeftChamfer: 8
  }

  XDGIcon {
    id: iconImage
    anchors.centerIn: parent
    asynchronous: true
    size: 18
    icon: SysInfo.iconName
  }

  WrappedMultiEffect {
    source: iconImage
    anchors.fill: iconImage
    colorization: 1
    colorizationColor: root.fgCol
    brightness: 0.7
    scale: root.isActive ? 1.2 : 1

    Behavior on scale {
      NAnim {
        duration: root.animDuration
      }
    }
    Behavior on colorizationColor {
      CAnim {
        duration: root.animDuration
      }
    }
  }
}
