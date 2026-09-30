import qs.modules.bar
import qs.modules.notifications as NOTIFICATIONS
import qs.modules.launcher as LAUNCHER
import qs.modules.bar.tray

import Quickshell
import QtQuick

FocusScope {
	id: root

	required property ShellScreen screen
	required property QsWindow win

	required property Bar bar

	property alias notificationsOverlay: notificationsOverlay

	TrayItemPopout {
		id: trayPopout
		systemTray: root.bar.systemTray // qmllint disable incompatible-type
		content: root // qmllint disable incompatible-type
	}

	LAUNCHER.LauncherWrapper {
		id: launcherWrapper

		anchors.centerIn: parent
		maxWidth: root.width
		content: root // qmllint disable incompatible-type
	}

	NOTIFICATIONS.Overlay {
		id: notificationsOverlay

		anchors {
			top: parent.top
			right: parent.right
		}

		maxHeight: root.height
	}

	NOTIFICATIONS.PanelWrapper {
		id: notificationsPanel
		statusModule: root.bar.statusItems
	}
}
