pragma ComponentBehavior: Bound

import qs.modules as Modules
import qs.utils

import org.nightshell.Utils
import org.nightshell.Hyprland
import org.nightshell.IpcServer

import Quickshell
import Quickshell.Wayland as QSWayland
import QtQuick

Item {
	id: root

	required property InstanceContext context
	readonly property Modules.Content content: context.content
	required property int maxWidth
	property bool isActive: false

	function toggleLauncher(targetScreen: ShellScreen) {
		const hmon = Hyprland.monitorsModel.values.find(m => m.name === targetScreen.name);
		if (targetScreen != root.content.screen && hmon) {
			root.isActive = false;
			return;
		}
		if (hmon) {
			const hwp = hmon.activeWorkspace;
			if (hwp.isFullScreen) {
				root.isActive = false;
				return;
			}
		}
		root.isActive = !root.isActive;
	}

	function closeLauncher() {
		root.isActive = false;
	}

	Connections {
		target: IPCServer
		function onLauncherToggleRequested() {
			const activeScreen = Hyprland.monitorsModel.focusedMonitor;
			const qsScreen = Quickshell.screens.find(s => s.name === activeScreen.name);
			root.toggleLauncher(qsScreen);
		}
	}

	Connections {
		target: root.context
		function onRequestToggleLauncher() {
			root.toggleLauncher(root.context.shellScreen);
		}
	}

	PanelWindow {
		anchors {
			left: true
			right: true
			top: true
			bottom: true
		}
		screen: root.content.screen
		color: "transparent"
		visible: contentLoader.active
		exclusionMode: ExclusionMode.Ignore
		QSWayland.WlrLayershell.keyboardFocus: root.isActive ? QSWayland.WlrKeyboardFocus.Exclusive : QSWayland.WlrKeyboardFocus.None
		QSWayland.WlrLayershell.layer: QSWayland.WlrLayer.Top
		QSWayland.WlrLayershell.namespace: "nightshell-launcher"

		MouseArea {
			anchors.fill: parent
			onClicked: root.closeLauncher()
			focus: true

			FocusGrabber {
				active: root.isActive
				onFocusLost: root.closeLauncher()
			}

			Loader {
				id: contentLoader
				readonly property bool shouldBeActive: root.isActive
				active: false
				anchors.centerIn: parent

				onShouldBeActiveChanged: {
					if (shouldBeActive)
						active = true;
				}

				sourceComponent: LauncherContent {
					id: launcherContent

					isActive: root.isActive
					maxWidth: root.content.width
					maxHeight: root.content.height
					onCloseRequested: {
						root.closeLauncher();
					}
					onExitAnimationFinished: {
						contentLoader.active = false;
					}
				}
			}
		}
	}
}
