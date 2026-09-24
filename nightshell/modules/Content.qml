import qs.modules.bar
import qs.modules.bar.tray

import Quickshell
import QtQuick

Item {
	id: root

	required property ShellScreen screen
	required property QsWindow win

	required property Bar bar

	TrayItemPopout {
		id: trayPopout
		systemTray: root.bar.systemTray
		content: root
	}
}
