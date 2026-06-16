pragma ComponentBehavior: Bound

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

	anchors.margins: padding

	implicitWidth: 320
	implicitHeight: Math.min(notifListView.contentHeight, parent.height - (padding * 2))

	Timer {
		id: cullTimer
		interval: 5000
		onTriggered: {
			notificationsModel.clear();
		}
	}

	ListModel {
		id: notificationsModel
	}

	ListView {
		id: notifListView

		acceptedButtons: Qt.NoButton
		interactive: false

		implicitWidth: root.implicitWidth
		implicitHeight: contentHeight

		clip: true
		spacing: Config.appearance.spacing.md

		delegate: NotifComponent {}
		model: notificationsModel

		add: Transition {
			ParallelAnimation {
				NAnim {
					property: "x"
					from: root.implicitWidth
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

	Connections {
		target: NotificationServer

		function onNotification(notification: Notification) {
			notificationsModel.insert(0, {
				idx: notification.id,
				appName: notification.appName,
				summary: notification.summary,
				body: notification.body
			});
			cullTimer.restart();
		}
	}

	component NotifComponent: MouseArea {
		id: notifItem

		required property int index
		required property int idx
		required property string appName
		required property string summary
		required property string body

		readonly property int padding: Config.appearance.padding.sm

		implicitWidth: ListView.view ? ListView.view.width : 0
		implicitHeight: rlayout.implicitHeight + padding * 2

		SequentialAnimation {
			id: removeAnimation
			PropertyAction {
				target: notifItem
				property: "ListView.delayRemove"
				value: true
			}
			NAnim {
				target: notifItem
				property: "x"
				to: notifListView.width
				duration: 400
			}
			PropertyAction {
				target: notifItem
				property: "ListView.delayRemove"
				value: false
			}
		}

		ListView.onRemove: removeAnimation.start()

		Rectangle {
			anchors.fill: parent
			color: Colors.colors.base

			radius: Config.appearance.rounding.sm
		}

		RowLayout {
			id: rlayout

			anchors {
				left: parent.left
				leftMargin: notifItem.padding
				right: parent.right
				rightMargin: notifItem.padding
				verticalCenter: parent.verticalCenter
			}

			ColumnLayout {
				Layout.fillWidth: true

				StyledText {
					Layout.fillWidth: true
					text: notifItem.appName

					font.family: Config.appearance.fontFamily.sans
					font.pointSize: Config.appearance.fontSize.md
					font.weight: 600

					elide: Text.ElideRight
				}

				StyledText {
					Layout.fillWidth: true

					text: notifItem.summary

					elide: Text.ElideRight
				}
			}
		}
	}
}
