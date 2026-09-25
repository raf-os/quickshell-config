import qs.components

import org.nightshell.Services
import org.nightshell.Configs

import QtQuick
import QtQuick.Effects

BaseStatus {
	id: root

	readonly property bool isConnected: NetworkService.isConnected
	readonly property bool isUncertain: NetworkService.isConnecting || NetworkService.isLimitedConnection
	readonly property bool isDisconnected: !isConnected && !isUncertain

	enabled: false

	Item {
		id: iconWrapper
		anchors.fill: parent

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

	SimpleGlowEffect {
		source: iconWrapper
		anchors.fill: iconWrapper
		colorization: 1
		colorizationColor: root.isConnected ? Colors.secondary : Colors.primary
		shadowColor: colorizationColor
	}
}
