pragma ComponentBehavior: Bound

import qs.modules.notifications
import qs.components
import qs.utils
import org.nightshell.Notifications
import MyShellPlugin
import MyShellPlugin.Configs
import Quickshell
import QtQuick
import QtQuick.Layouts

MouseArea {
	id: root

	required property ShellScreen screen
	required property OpenPanels openPanels

	readonly property int padding: Config.appearance.padding.md
	readonly property int desiredWidth: 320
	readonly property real maxHeight: parent.height - (padding * 2)

	// anchors.margins: padding

	implicitWidth: sidebarLoader.item ? (sidebarLoader.item as Item)?.width : 0

	// anchors.bottom: sidebarLoader.active ? parent.bottom : undefined

	preventStealing: true
	acceptedButtons: Qt.NoButton
	hoverEnabled: true

	function togglePanel() {
		root.openPanels.notifications = !root.openPanels.notifications;
	}

	onEntered: {
		TempNotifications.stopCullTimer();
	}

	onExited: {
		TempNotifications.startCullTimer();
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

	// Overlay {
	// 	id: notifOverlay
	//
	// 	isActive: root.openPanels.notifications === false
	// 	maxHeight: root.maxHeight
	// }

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
