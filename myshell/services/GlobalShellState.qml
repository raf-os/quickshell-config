pragma Singleton

import org.nightshell.IpcServer
import Quickshell
import QtQuick

Singleton {
	id: root

	property var windowScreenMap: new Map()
	readonly property list<ShellScreen> allScreens: Quickshell.screens

	property alias launcherScreen: props.launcherScreen
	readonly property bool isLauncherOpen: props.launcherScreen !== null

	property alias commandCenterScreen: props.commandCenterScreen
	readonly property bool isCommandCenterOpen: props.commandCenterScreen !== null

	property string desiredLauncherTab: ""

	signal commandCenterForward
	signal commandCenterBackward

	function getCurrentScreen(): ShellScreen {
		return Quickshell.screens.find(s => s.name === Hypr.focusedMonitor.name);
	}

	Connections {
		target: IPCServer

		function onLauncherToggleRequested() {
			if (root.isLauncherOpen) {
				root.closeLauncher();
				return;
			}

			const screen = root.getCurrentScreen();
			if (screen) {
				root.openLauncher(screen);
			}
		}

		function onLauncherOpenRequested() {
			const screen = root.getCurrentScreen();
			if (screen) {
				root.openLauncher(screen);
			}
		}
	}

	PersistentProperties {
		id: props

		property ShellScreen launcherScreen: null
		property ShellScreen commandCenterScreen: null
	}

	onAllScreensChanged: {
		// Re-evaluate validity of properties
		if (!allScreens.includes(props.launcherScreen)) {
			props.launcherScreen = null;
		}
		if (!allScreens.includes(props.commandCenterScreen)) {
			props.commandCenterScreen = null;
		}
	}

	function openCommandCenter(screen: ShellScreen): bool {
		if (isCommandCenterOpen) {
			root.commandCenterForward();
		} else {
			props.commandCenterScreen = screen;
			closeLauncher();
		}
	}

	function toggleCommandCenter(screen: ShellScreen): bool {
		if (!isCommandCenterOpen) {
			props.commandCenterScreen = screen;
			closeLauncher();
		} else {
			closeCommandCenter();
		}
	}

	function closeCommandCenter(): void {
		props.commandCenterScreen = null;
	}

	function closeLauncher(): void {
		props.launcherScreen = null;
	}

	function openLauncher(screen: ShellScreen): bool {
		desiredLauncherTab = "";
		closeCommandCenter();
		props.launcherScreen = screen;
	}

	function openLauncherWithArgs(screen: ShellScreen, arg: string) {
		desiredLauncherTab = arg;
		closeCommandCenter();
		props.launcherScreen = screen;
	}

	function registerWindowToScreen(window: QsWindow, screen: ShellScreen): bool {
		return windowScreenMap.set(window, screen);
	}

	function unregisterWindow(window: QsWindow): bool {
		return windowScreenMap.delete(window);
	}

	function getAssociatedScreen(window: QsWindow): ShellScreen {
		const screen = windowScreenMap.get(window);
		if (screen)
			return screen;
		return null;
	}
}
