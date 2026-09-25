pragma Singleton

import org.nightshell.Hyprland
import Quickshell
import QtQuick

Singleton {
	id: root

	readonly property bool isGamemode: props.isActive

	function toggleGamemode() {
		props.isActive = !props.isActive;
	}

	PersistentProperties {
		id: props
		reloadableId: "gamemode_props"

		property bool isActive: false

		function applyGamemode() {
			Hyprland.applyOptions({
				"animations:enabled": false,
				"general": {
					"gaps_in": 0,
					"gaps_out": 0,
					"border_size": 1
				},
				"decoration": {
					"shadow:enabled": false,
					"blur:enabled": false,
					"glow:enabled": false
				}
			});
		}

		onIsActiveChanged: {
			if (isActive) {
				props.applyGamemode();
			} else {
				Hyprland.reloadOptions();
			}
		}
	}

	Connections {
		target: Hyprland.eventHandler

		function onConfigReloaded() {
			if (props.isActive) {
				props.applyGamemode();
			}
		}
	}
}
