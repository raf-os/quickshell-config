import qs.utils
import qs.modules.bar
import qs.modules.notifications as NOTIFICATIONS
import qs.modules.launcher as LAUNCHER
import qs.modules.osd as OSD
import qs.modules.bar.tray

import org.nightshell.Hyprland

import Quickshell
import QtQuick

FocusScope {
	id: root

	required property InstanceContext context
	readonly property ShellScreen screen: context.shellScreen
	readonly property Bar bar: context.bar
	readonly property QsWindow win: context.win

	readonly property HyprMonitor hyprMonitor: Hyprland.monitorsModel.values.find(m => m.name === screen.name) ?? null

	property alias notificationsOverlay: notificationsOverlay
	property alias launcherWrapper: launcherWrapper

	TrayItemPopout {
		id: trayPopout
		systemTray: root.bar.systemTray // qmllint disable incompatible-type
		content: root // qmllint disable incompatible-type
	}

	LAUNCHER.LauncherWrapper {
		id: launcherWrapper

		anchors.centerIn: parent
		maxWidth: root.width
		context: root.context
	}

	OSD.OSD {
		id: osd
		context: root.context
		anchors {
			bottom: parent.bottom
			right: parent.right
		}
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
