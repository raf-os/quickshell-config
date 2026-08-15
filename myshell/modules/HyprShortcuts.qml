import qs.components.utils
import qs.services
import qs.utils
import QtQuick
import Quickshell

// Makes use of hyprland's "global shortcuts" so you can reroute hotkeys
// to this shell through quickshell's IPC

Scope {
	id: root

	// property bool startMenuInterrupted
	readonly property bool hasFullscreen: Hypr.focusedWorkspace?.toplevels.values.some(t => t.lastIpcObject.fullscreen === 2) ?? false

	function getActiveScreen(): ShellScreen {
		return Quickshell.screens.find(s => s.name === Hypr.focusedMonitor.name);
	}

	CustomShortcut {
		name: "launcher"
		description: "Toggle launcher"

		// onPressed: root.startMenuInterrupted = false
		onReleased: {
			if (!root.hasFullscreen) {
				const activeMonitor = Hypr.focusedMonitor;
				const activeScreen = Quickshell.screens.find(s => s.name === activeMonitor.name);
				if (activeScreen) {
					if (GlobalShellState.launcherScreen === activeScreen) {
						GlobalShellState.closeLauncher();
					} else {
						GlobalShellState.openLauncher(activeScreen);
					}
				}
			}
		}
	}

	CustomShortcut {
		name: "launcher-windows"
		description: "Open launcher on the 'windows' tab"

		onReleased: {
			if (!root.hasFullscreen) {
				const screen = root.getActiveScreen();
				if (screen) {
					GlobalShellState.openLauncherWithArgs(screen, "windows");
				}
			}
		}
	}

	CustomShortcut {
		name: "command-center"
		description: "Open command center"

		onReleased: {
			const screen = root.getActiveScreen();
			if (screen) {
				GlobalShellState.openCommandCenter(screen);
			}
		}
	}

	// CustomShortcut {
	// 	//TODO: this
	// 	name: "startmenuCommand"
	// 	description: "Toggle start menu in command mode"
	//
	// 	onPressed: {
	// 		root.startMenuInterrupted = true;
	// 	}
	// 	onReleased: {
	// 		const openPanels = PanelService.getForActive();
	// 		if (openPanels.startmenu === true)
	// 			return;
	// 		openPanels.startmenu = true;
	// 		openPanels.desiredStartMenuTab = "command";
	// 	}
	// }
	//
	// CustomShortcut {
	// 	name: "startMenuInterrupt"
	// 	description: "Interrupt start menu keybind"
	// 	onPressed: root.startMenuInterrupted = true
	// }

	CustomShortcut {
		name: "toggleGamemode"
		description: "Toggles game mode on/off"
		onPressed: GlobalStateManager.toggleGamemodeState()
	}
}
