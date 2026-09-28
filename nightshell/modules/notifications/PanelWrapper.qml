pragma ComponentBehavior: Bound
import qs.modules.bar.status as STATUSMODULE

import org.nightshell.Utils
import org.nightshell.Notifications
import org.nightshell.Configs

import QtQuick

Item {
	id: root

	required property STATUSMODULE.Main statusModule
	readonly property bool isActive: statusModule.currentActive && statusModule.currentActive.name === "notifications"

	FocusGrabber {
		active: root.isActive
		onFocusLost: {
			root.statusModule.clearActive();
		}
	}

	anchors {
		top: parent.top
		right: parent.right
		bottom: parent.bottom
	}

	implicitWidth: isActive ? sidebarLoader.width : 0

	Loader {
		id: sidebarLoader
		asynchronous: true

		anchors {
			top: parent.top
			bottom: parent.bottom
			right: parent.right
		}

		readonly property bool shouldBeActive: root.isActive
		active: false

		onShouldBeActiveChanged: {
			if (shouldBeActive) {
				TempNotifications.clear();
				active = true;
			}
		}

		sourceComponent: PanelContent {
			id: panelContent
			isActive: root.isActive

			onExitAnimationComplete: {
				sidebarLoader.active = false;
			}
		}
	}
}
