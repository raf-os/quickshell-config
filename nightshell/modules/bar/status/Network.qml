pragma ComponentBehavior: Bound

import qs.components

import org.nightshell.Services
import org.nightshell.Configs

import QtQuick
import QtQuick.Effects

BaseStatus {
  id: root

  name: "network"

  readonly property bool isConnected: NetworkService.isConnected
  readonly property bool isUncertain: NetworkService.isConnecting || NetworkService.isLimitedConnection
  readonly property bool isDisconnected: !isConnected && !isUncertain

  enabled: false

  Item {
    id: iconWrapper
    anchors.fill: parent

    layer.enabled: true
    layer.effect: SimpleGlowEffect {
      colorization: 1
      colorizationColor: root.isConnected ? Colors.secondary : Colors.primary
      shadowColor: colorizationColor
    }

    StatusIcon {
      icon: "network_disconnected"
      visible: root.isDisconnected
    }

    StatusIcon {
      icon: "network_uncertain"
      visible: root.isUncertain
    }

    StatusIcon {
      icon: "network_ethernet"
      visible: root.isConnected
    }
  }
}
