pragma ComponentBehavior: Bound

import qs.modules as Modules

import Quickshell
import QtQuick

Item {
	id: root

	required property Modules.Content content
	property bool isActive: false

	function toggleLauncher(targetScreen: ShellScreen) {
		if (targetScreen != root.content.screen) {
			root.isActive = false;
			return;
		}
		root.isActive = !root.isActive;
	}

	function closeLauncher() {
		root.isActive = false;
	}

	Loader {
		readonly property bool shouldBeActive: root.isActive
		active: false

		onShouldBeActiveChanged: {
			if (shouldBeActive)
				active = true;
		}

		sourceComponent: LauncherContent {
			id: launcherContent

			isActive: root.isActive
			maxWidth: root.content.width
			maxHeight: root.content.height
			onCloseRequested: root.closeLauncher()
		}
	}
}
