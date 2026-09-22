import Quickshell
import Quickshell.Wayland
import QtQuick

Variants {
	id: root

	model: Quickshell.screens

	Scope {
		id: scope
		required property ShellScreen modelData

		PanelWindow { // qmllint disable uncreatable-type
			id: bgWin
			screen: scope.modelData

			WlrLayershell.layer: WlrLayer.Background
			WlrLayershell.namespace: `nightshell-background`
			exclusionMode: ExclusionMode.Ignore

			anchors {
				left: true
				right: true
				top: true
				bottom: true
			}

			Rectangle {
				anchors.fill: parent
				color: "black"
			}

			Wallpaper {
				id: wallpaperItem
				anchors.fill: parent
			}
		}
	}
}
