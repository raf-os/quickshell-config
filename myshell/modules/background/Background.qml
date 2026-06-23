import MyShellPlugin.Configs
import Quickshell
import Quickshell.Wayland
import QtQuick

Loader {
	id: root
	active: Config.wallpaper.enabled

	sourceComponent: Variants {
		model: Quickshell.screens

		Scope {
			id: scope

			required property ShellScreen modelData

			PanelWindow { // qmllint disable uncreatable-type
				id: backgroundWindow

				screen: scope.modelData

				WlrLayershell.layer: WlrLayer.Background
				WlrLayershell.namespace: `myshell-background`

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

				Wallpaper {}
			}
		}
	}
}
