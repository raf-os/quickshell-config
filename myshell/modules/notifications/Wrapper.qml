pragma ComponentBehavior: Bound

import qs.modules.notifications
import qs.utils
import MyShellPlugin.Configs
import Quickshell
import QtQuick

Item {
	id: root

	required property ShellScreen screen
	required property OpenPanels openPanels

	readonly property int padding: Config.appearance.padding.md
	readonly property int desiredWidth: Config.notification.sidebarWidth
	readonly property real maxHeight: parent.height - (padding * 2)

	// anchors.margins: padding

	implicitWidth: sidebarLoader.item ? (sidebarLoader.item as Item)?.width : 0

	// anchors.bottom: sidebarLoader.active ? parent.bottom : undefined

	function togglePanel() {
		root.openPanels.notifications = !root.openPanels.notifications;
	}

	Connections {
		target: root.openPanels
		enabled: Config.notification.enabled

		function onNotificationsChanged() {
			if (root.openPanels.notifications === true) {
				TempNotifications.clearNotifications();
				sidebarLoader.shouldBeActive = true;
			} else {
				sidebarLoader.shouldBeActive = false;
			}
		}
	}

	Loader {
		id: sidebarLoader

		property bool shouldBeActive: false
		active: false

		anchors {
			top: parent.top
			bottom: parent.bottom
			right: parent.right
		}

		onShouldBeActiveChanged: {
			if (shouldBeActive === true)
				active = true;
		}

		sourceComponent: Content {
			isActive: sidebarLoader.shouldBeActive
			desiredWidth: root.desiredWidth

			onReadyToUnload: {
				sidebarLoader.active = false;
				root.openPanels.notifications = false;
			}

			onLostFocus: {
				sidebarLoader.shouldBeActive = false;
			}
		}
	}
}
