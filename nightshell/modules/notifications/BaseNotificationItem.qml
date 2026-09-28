import org.nightshell.Notifications
import org.nightshell.Configs

import QtQml
import QtQuick

MouseArea {
	id: root

	required property int index
	required property Notification modelData

	property int animDuration: 300
	property int padding: Styles.padding_md

	property int notifIdx
	property int urgency
	property string appName
	property string appIcon
	property string summary
	property string body
	property string imageUrl
	property bool hasActionIcons
	property list<NotificationAction> notificationActions

	signal requestClose(reason: int)

	Binding {
		when: (root.modelData !== null && root.modelData !== undefined)

		root.notifIdx: root.modelData.id
		root.urgency: root.modelData.urgency
		root.appName: root.modelData.appName
		root.appIcon: root.modelData.appIcon
		root.summary: root.modelData.summary
		root.body: root.modelData.body
		root.imageUrl: root.modelData.imageUrl
		root.hasActionIcons: root.modelData.hasActionIcons
		root.notificationActions: root.modelData.actions
	}

	Connections {
		target: root.modelData

		function onClosed(reason: int): void {
			root.requestClose(reason);
		}
	}
}
