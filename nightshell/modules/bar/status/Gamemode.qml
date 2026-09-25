import qs.services
import qs.components

import org.nightshell.Configs

import QtQuick

BaseStatus {
	id: root

	StatusIcon {
		id: icon
		visible: false
		icon: GameMode.isActive ? "gamemode_on" : "gamemode_off"
	}

	SimpleGlowEffect {
		source: icon
		anchors.fill: icon
		colorization: 1
		colorizationColor: GameMode.isActive ? Colors.secondary : Colors.primary
		shadowColor: Colors.primary
	}

	onClicked: {
		GameMode.toggleGamemode();
	}
}
