pragma ComponentBehavior: Bound

import qs.modules as Modules

import org.nightshell.Utils
import org.nightshell.Hyprland
import org.nightshell.IpcServer

import Quickshell
import Quickshell.Wayland as QSWayland
import QtQuick

Item {
	id: root

	required property Modules.Content content
	required property int maxWidth
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

	Connections {
		target: IPCServer
		function onLauncherToggleRequested() {
			const activeScreen = Hyprland.monitorsModel.focusedMonitor;
			const qsScreen = Quickshell.screens.find(s => s.name === activeScreen.name);
			root.toggleLauncher(qsScreen);
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
