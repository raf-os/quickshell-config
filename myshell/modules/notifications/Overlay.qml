pragma ComponentBehavior: Bound

import qs.modules.notifications
import qs.components
import qs.utils
import org.nightshell.Notifications
import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick

MouseArea {
	id: root

	required property bool isActive
	required property real maxHeight
	required property OpenPanels openPanels

	readonly property int padding: Config.appearance.padding.md

	implicitWidth: Config.notification.sidebarWidth - padding
	implicitHeight: notifListView.implicitHeight

	anchors.topMargin: padding
	anchors.rightMargin: padding

	hoverEnabled: true
	preventStealing: true
	acceptedButtons: Qt.NoButton

	onEntered: {
		TempNotifications.stopCullTimer();
	}

	onExited: {
		TempNotifications.startCullTimer();
	}

	ListView {
		id: notifListView

		anchors {
			top: parent.top
			left: parent.left
			right: parent.right
		}

		acceptedButtons: Qt.NoButton
		interactive: false

		implicitWidth: root.implicitWidth
		implicitHeight: Math.min(contentHeight, root.maxHeight - root.padding * 2)

		clip: true
		spacing: Config.appearance.spacing.md

		delegate: NotificationItem {
			id: notificationItemDelegate

			isTemporary: true

			onActivated: idx => {
				root.openPanels.openNotificationsAtId(idx);
				TempNotifications.clearNotifications();
			}

			onCloseNotification: {
				if (notificationItemDelegate.modelData) {
					notificationItemDelegate.modelData.dismiss();
				}
			}

			onForceClose: {
				TempNotifications.removeNotificationById(notificationItemDelegate.index);
			}

			bodyContent: StyledText {
				id: notifBody

				anchors {
					top: parent.top
					left: parent.left
					right: parent.right
				}

				text: notificationItemDelegate.summary !== "" ? notificationItemDelegate.summary : notificationItemDelegate.body

				font.family: Config.appearance.fontFamily.sans
				font.italic: true
				font.pointSize: Config.appearance.fontSize.sm
				opacity: 0.75

				elide: Text.ElideRight
				maximumLineCount: 2
			}
		}
		model: TempNotifications.model

		add: Transition {
			ParallelAnimation {
				NAnim {
					property: "x"
					from: root.width
					to: 0
					duration: 400
				}

				NAnim {
					property: "opacity"
					from: 0
					to: 1
					duration: 400
				}
			}
		}
	}
}
