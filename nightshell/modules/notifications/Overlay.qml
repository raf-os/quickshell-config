import qs.components
import qs.modules.notifications

import org.nightshell.Configs

import QtQuick
import QtQuick.Layouts

MouseArea {
	id: root

	required property int maxHeight

	readonly property int padding: Styles.padding_md

	hoverEnabled: true

	implicitWidth: 300 - padding * 2
	implicitHeight: listView.implicitHeight

	onEntered: {
		TempNotifications.stopCullTimer();
	}
	onExited: {
		TempNotifications.startCullTimer();
	}

	ListView {
		id: listView

		acceptedButtons: Qt.NoButton
		clip: true

		anchors {
			left: parent.left
			right: parent.right
			top: parent.top
		}

		implicitHeight: Math.min(contentHeight, root.maxHeight - root.padding * 2)

		model: TempNotifications.model
		delegate: BaseNotificationItem {
			id: notificationDelegate

			implicitWidth: ListView.view ? ListView.view.width : 0
			implicitHeight: mainLayout.height

			onClicked: {
				TempNotifications.removeNotification(notificationDelegate.index);
			}

			RowLayout {
				id: mainLayout
				anchors {
					left: parent.left
					right: parent.right
					top: parent.top
				}

				NotificationIcon {
					id: notifIcon
					Layout.alignment: Qt.AlignVCenter

					appName: notificationDelegate.appName
					appIcon: notificationDelegate.appIcon
					imageUrl: notificationDelegate.imageUrl
				}

				NotificationLayout {
					title: notificationDelegate.appName

					onCloseRequested: reason => {
						if (!notificationDelegate.modelData)
							return;

						TempNotifications.removeNotification(notificationDelegate.index);
						notificationDelegate.modelData.dismiss();
					}

					StyledText {
						id: notificationBody

						Layout.fillWidth: true

						text: notificationDelegate.summary !== "" ? notificationDelegate.summary : notificationDelegate.body

						font.pointSize: Styles.text_sm
						elide: Text.ElideRight
						maximumLineCount: 2
					}
				}
			}
		}
	}
}
