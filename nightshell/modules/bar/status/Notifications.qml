import qs.components

import org.nightshell.Notifications
import org.nightshell.Configs

import QtQuick
import QtQuick.Effects

BaseStatus {
	id: root

	name: "notifications"

	readonly property int notificationAmount: NotificationServer.model.values.length // qmllint disable unresolved-type
	readonly property bool hasNotifications: notificationAmount > 0

	Item {
		id: iconsWrapper
		anchors.fill: parent
		StatusIcon {
			icon: "notification_off"
			visible: !root.hasNotifications
		}
		StatusIcon {
			icon: "notification_on"
			visible: root.hasNotifications
		}
	}

	SimpleGlowEffect {
		source: iconsWrapper
		anchors.fill: iconsWrapper
		colorization: 1
		colorizationColor: root.hasNotifications ? Colors.secondary : Colors.primary
		shadowColor: colorizationColor
	}
}
