import qs.components

import org.nightshell.Configs

import QtQuick
import QtQuick.Effects
import QtQuick.Templates as T

Item {
  id: root

  property color bgColor: Colors.primaryMuted
  property color fgColor: Colors.primary
  property color handleColor: Colors.accent
  property color handleInactiveColor: Colors.secondary

  property bool mirror: false

  property int handleSize: 18
  property int barHeight: 8

  property alias pressed: control.pressed
  property alias from: control.from
  property alias to: control.to
  property alias value: control.value

  property alias handle: control.handle

  readonly property bool isHorizontal: width >= height
  // Use this to undo the rotation
  readonly property int rotationAngle: isHorizontal ? (mirror ? 180 : 0) : (mirror ? 270 : 90)

  implicitWidth: handleSize
  implicitHeight: handleSize

  signal moved

  T.Slider {
    id: control

    onMoved: root.moved()

    width: Math.max(root.width, root.height)
    height: Math.min(root.width, root.height)

    transform: [
      Rotation {
        origin.x: root.mirror ? 0 : control.height / 2
        origin.y: root.mirror ? 0 : control.height / 2
        angle: root.rotationAngle
      },
      Translate {
        x: root.isHorizontal ? (root.mirror ? control.width : 0) : 0
        y: root.isHorizontal ? (root.mirror ? control.height : 0) : root.mirror ? control.width : 0
      }
    ]

    background: Item {
      id: bgRoot

      Rectangle {
        id: barBackground

        anchors {
          left: parent.left
          right: parent.right
          verticalCenter: parent.verticalCenter
        }
        implicitHeight: root.barHeight
        color: root.bgColor
      }

      Rectangle {
        id: barBackgroundBorder

        anchors.fill: barBackground

        color: "transparent"
        border.width: 1
        border.color: barForeground.color
      }

      RectangularShadow {
        anchors.fill: barForeground
        color: Qt.alpha(barForeground.color, 0.75)
        blur: 12
        spread: 2
        opacity: Math.max(1, control.visualPosition * parent.width / blur)
      }

      Rectangle {
        id: barForeground

        anchors {
          left: parent.left
          verticalCenter: parent.verticalCenter
          margins: 2
        }

        width: control.visualPosition * parent.width - 4
        height: root.barHeight - 4
        color: root.fgColor
      }
    }

    handle: MouseArea {
      acceptedButtons: Qt.NoButton
      enabled: root.enabled
      cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

      x: control.visualPosition * (control.availableWidth - width)
      y: control.availableHeight / 2 - height / 2
      implicitWidth: root.handleSize
      implicitHeight: root.handleSize

      RectangularShadow {
        anchors.fill: handleFg
        anchors.margins: root.pressed ? -2 : 0
        color: Qt.alpha(handleFg.color, 0.75)
        blur: 12
        spread: 2
      }

      Rectangle {
        id: handleFg
        anchors.fill: parent
        color: root.pressed ? root.handleColor : root.handleInactiveColor

        Behavior on color {
          CAnim {}
        }
      }
    }
  }
}
