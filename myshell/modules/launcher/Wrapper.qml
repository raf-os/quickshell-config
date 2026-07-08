pragma ComponentBehavior: Bound

import qs.components
import qs.services
import Quickshell
import Quickshell.Hyprland
import QtQuick

Item {
	id: root

	required property Item panelsItem
	required property ShellScreen screen

	readonly property bool shouldBeActive: GlobalShellState.launcherScreen === screen

	// focus: shouldBeActive

	// property alias region: region.region
	property alias region: region

	// AutoTransformMap {
	// 	id: region
	// 	reference: root.panelsItem
	// 	target: contentLoader.item ? (contentLoader.item as Item) : null
	// }

	HyprlandFocusGrab {
		id: focusGrabber
		windows: [QsWindow.window]
		active: root.shouldBeActive
		onCleared: {
			GlobalShellState.closeLauncher();
		}
	}

	Region {
		id: region

		readonly property Content target: (contentLoader.item as Content) ? (contentLoader.item as Content) : null

		x: target ? root.x + contentLoader.x + target.x : 0
		y: target ? root.y + contentLoader.y + target.y : 0
		width: target ? target.width : 0
		height: target ? target.height : 0
	}

	Loader {
		id: contentLoader

		active: root.shouldBeActive

		anchors.centerIn: parent

		sourceComponent: Content {
			id: launcherContent

			boundsWidth: root.panelsItem.width
			boundsHeight: root.panelsItem.height

			onCloseLauncherRequested: {
				GlobalShellState.closeLauncher();
			}
		}
	}
}
