pragma ComponentBehavior: Bound
import qs.components

import org.nightshell.Configs
import org.nightshell.Notifications

import QtQuick
import QtQuick.Layouts

MouseArea {
	id: root

	required property bool isActive

	readonly property bool hasNotifications: NotificationServer.model.values.length > 0 // qmllint disable unresolved-type
	readonly property int desiredWidth: 400
	readonly property int animDuration: 300

	focus: isActive

	signal exitAnimationComplete
	signal requestClose

	states: State {
		name: "active"
		when: root.isActive

		PropertyChanges {
			root.implicitWidth: root.desiredWidth
		}
	}

	transitions: [
		Transition {
			to: "active"
			NAnim {
				property: "implicitWidth"
				duration: root.animDuration
			}
		},
		Transition {
			to: ""
			SequentialAnimation {
				NAnim {
					property: "implicitWidth"
					duration: root.animDuration
				}
				ScriptAction {
					script: root.exitAnimationComplete()
				}
			}
		}
	]

	anchors {
		top: parent.top
		bottom: parent.bottom
		right: parent.right
	}

	Rectangle {
		anchors.fill: listView
		color: Qt.alpha(Colors.primaryContent, 0.5)
		border.width: 1
		border.color: Colors.primary
	}

	Item {
		id: notificationsHeader

		implicitWidth: root.desiredWidth
		implicitHeight: header.height

		anchors {
			top: parent.top
			left: parent.left
		}

		SimpleGlowEffect {
			source: header
			anchors.fill: header
			shadowColor: Colors.primary
		}

		StyledText {
			id: header

			anchors {
				left: parent.left
				verticalCenter: parent.verticalCenter
			}

			text: "Notifications"

			font.pointSize: Styles.text_lg
			font.weight: 600

			color: Colors.primary
		}

		MouseArea {
			id: clearBtn
			implicitWidth: clearBtnText.width
			enabled: root.hasNotifications
			cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
			hoverEnabled: true

			anchors {
				top: parent.top
				right: parent.right
				bottom: parent.bottom
			}

			Rectangle {
				anchors.fill: clearBtnText
				color: clearBtn.containsMouse ? Colors.primary : Colors.primaryContent
				border.width: 1
				border.color: Colors.primary
			}

			StyledText {
				id: clearBtnText
				text: "CLEAR"
				leftPadding: 4
				rightPadding: 4
				topPadding: 2
				bottomPadding: 2
				font.pointSize: Styles.text_sm
				font.weight: 600

				anchors {
					right: parent.right
					// verticalCenter: parent.verticalCenter
				}
			}
		}
	}

	ListView {
		id: listView
		clip: true

		readonly property int componentWidth: root.desiredWidth - anchors.margins * 2

		model: NotificationServer.model
		spacing: Styles.spacing_xl

		anchors {
			top: notificationsHeader.bottom
			bottom: parent.bottom
			left: parent.left
			right: parent.right
			margins: Styles.padding_md
		}

		delegate: BaseNotificationItem {
			id: notificationDelegate

			implicitWidth: listView.componentWidth
			implicitHeight: mainLayout.implicitHeight

			RowLayout {
				id: mainLayout
				anchors {
					left: parent.left
					right: parent.right
					top: parent.top
					margins: Styles.padding_md
				}

				spacing: Styles.spacing_xl

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

						notificationDelegate.modelData.dismiss();
					}

					StyledText {
						id: notificationBody

						Layout.fillWidth: true
						text: notificationDelegate.body !== "" ? notificationDelegate.body : notificationDelegate.summary

						font.pointSize: Styles.text_sm

						wrapMode: Text.Wrap
						maximumLineCount: 10
						elide: Text.ElideRight
					}

					ColumnLayout {
						id: bodyActionsLayout

						Layout.fillWidth: true

						Repeater {
							model: notificationDelegate.notificationActions
							Layout.fillWidth: true

							delegate: ActionButton {
								required property NotificationAction modelData

								Layout.fillWidth: true
								hasIcon: notificationDelegate.hasActionIcons

								onInvoked: {
									modelData.invoke();
									root.requestClose();
								}
							}
						}
					}
				}
			}
		}
	}

	Item {
		id: noNotificationDisplay

		anchors {
			top: parent.top
			left: parent.left
			bottom: parent.bottom
		}

		implicitWidth: root.desiredWidth

		opacity: root.hasNotifications ? 0 : 1

		StyledText {
			anchors.fill: parent

			text: "No new notifications."

			font.pointSize: Styles.text_md

			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
		}
	}

	component ActionButton: MouseArea {
		id: actionButtonComponent

		required property string text
		required property string identifier
		required property bool hasIcon

		readonly property int paddingH: Styles.padding_md
		readonly property int paddingV: Styles.padding_md

		cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
		hoverEnabled: true

		implicitHeight: actionButtonContent.implicitHeight + paddingV * 2

		signal invoked

		onClicked: {
			invoked();
		}

		Rectangle {
			anchors.fill: parent
			color: Colors.primaryContent
			border.width: 1
			border.color: Colors.primary
		}

		RowLayout {
			id: actionButtonContent
			anchors {
				left: parent.left
				leftMargin: actionButtonComponent.paddingH
				right: parent.right
				rightMargin: actionButtonComponent.paddingH
				verticalCenter: parent.verticalCenter
			}

			Loader {
				id: actionButtonIconWrapper
				active: actionButtonComponent.hasIcon
				Layout.fillHeight: true

				sourceComponent: Image {
					id: actionButtonIcon
					asynchronous: true
					anchors.verticalCenter: parent.verticalCenter
					width: actionButtonContent.implicitHeight
					height: width
					source: `image://qicons/qt/${actionButtonComponent.identifier}`
				}
			}

			StyledText {
				id: actionButtonText

				Layout.fillWidth: true
				Layout.fillHeight: true

				text: actionButtonComponent.text
				elide: Text.ElideRight

				font.pointSize: Styles.text_sm
				font.weight: 600

				verticalAlignment: Text.AlignVCenter
			}
		}
	}
}
