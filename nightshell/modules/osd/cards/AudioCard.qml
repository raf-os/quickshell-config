pragma ComponentBehavior: Bound

import qs.components
import qs.services
import ".."

import org.nightshell.Configs
import org.nightshell.Services.Pipewire

import QtQuick
import QtQuick.Layouts
import QtQuick.Effects

CardBase {
  id: root

  readonly property bool isMuted: AudioService.defaultSinkMuted
  readonly property real volume: AudioService.defaultSinkVolume

  CardIcon {
    icon: AudioService.sinkIcon
  }

  CardContent {
    title: "Volume"
    content: root.isMuted ? "Muted" : ""

    Loader {
      active: !root.isMuted
      visible: active
      Layout.fillWidth: true
      sourceComponent: Item {
        implicitHeight: 12

        Rectangle {
          anchors.fill: parent
          color: Colors.secondaryContent
        }

        SimpleGlowEffect {
          source: fgBar
          anchors.fill: fgBar
          shadowColor: fgBar.color
        }

        Rectangle {
          id: fgBar
          visible: !fgBar.enabled
          implicitWidth: Math.min(1, Math.max(root.volume, 0)) * parent.width
          anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
          }
          color: Colors.secondary
        }
      }
    }
  }
}
