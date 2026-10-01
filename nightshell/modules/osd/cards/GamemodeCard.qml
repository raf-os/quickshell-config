import ".."
import qs.services

import QtQuick

CardBase {
	id: root

	readonly property bool isActive: GameMode.isActive

	CardIcon {
		icon: root.isActive ? "gamemode_on" : "gamemode_off"
	}

	CardContent {
		title: root.isActive ? "Gamemode on" : "Gamemode off"
	}
}
