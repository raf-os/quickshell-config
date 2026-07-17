pragma Singleton

import Quickshell
import QtQuick

Singleton {
	id: root

	property var windowScreenMap: new Map()
	readonly property list<ShellScreen> allScreens: Quickshell.screens

	property alias launcherScreen: props.launcherScreen
	readonly property bool isLauncherOpen: props.launcherScreen !== null

	property string desiredLauncherTab: ""

	PersistentProperties {
		id: props

		property ShellScreen launcherScreen: null
	}

	onAllScreensChanged: {
		// Re-evaluate validity of properties
		if (!allScreens.includes(props.launcherScreen)) {
			props.launcherScreen = null;
		}
	}

	function closeLauncher(): void {
		props.launcherScreen = null;
	}

	function openLauncher(screen: ShellScreen): bool {
		// if (!root.allScreens.includes(screen))
		// 	return false;
		desiredLauncherTab = "";
		props.launcherScreen = screen;
	}

	function openLauncherWithArgs(screen: ShellScreen, arg: string) {
		desiredLauncherTab = arg;
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
