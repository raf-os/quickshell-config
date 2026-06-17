pragma ComponentBehavior: Bound

import qs.modules.notifications
import qs.components
import qs.utils
import org.nightshell.Notifications
import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick

Item {
	id: root

	required property bool isActive
	required property real maxHeight

	readonly property int padding: Config.appearance.padding.md

	implicitWidth: 320 - padding
	implicitHeight: notifListView.implicitHeight

	anchors.topMargin: padding
	anchors.rightMargin: padding

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
			onCloseNotification: {
				TempNotifications.removeNotificationById(index);
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
