import qs.components
import org.nightshell.Notifications
import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick
import QtQuick.Layouts

MouseArea {
	id: root

	required property int index // list ID
	required property Notification modelData
	property int enterExitDuration: 300
	property int padding: Config.appearance.padding.md

	readonly property int notifIdx: modelData.id
	readonly property string appName: modelData.appName
	readonly property string appIcon: modelData.appIcon
	readonly property string summary: modelData.summary
	readonly property string body: modelData.body

	readonly property int desiredWidth: ListView.view ? ListView.view.width : 0

	implicitWidth: desiredWidth
	implicitHeight: mainLayout.implicitHeight + padding * 2

	signal closeNotification

	SequentialAnimation {
		id: removeAnimation
		PropertyAction {
			target: root
			property: "ListView.delayRemove"
			value: true
		}
		NAnim {
			target: root
			property: "x"
			to: root.desiredWidth
			duration: 400
		}
		PropertyAction {
			target: root
			property: "ListView.delayRemove"
			value: false
		}
	}
	ListView.onRemove: removeAnimation.start()

	Rectangle {
		id: backgroundRect

		anchors.fill: parent
		color: Colors.colors.base
		radius: Config.appearance.rounding.sm
	}

	RowLayout {
		id: mainLayout

		anchors {
			left: parent.left
			right: parent.right
			verticalCenter: parent.verticalCenter
			leftMargin: root.padding
			rightMargin: root.padding
		}

		ColumnLayout {
			id: layoutTextContent
			Layout.fillWidth: true

			Item {
				id: headerWrapper
				Layout.fillWidth: true

				implicitHeight: Math.max(appNameText.height, closeButton.height)

				StyledText {
					id: appNameText
					text: root.appName

					font.family: Config.appearance.fontFamily.sans
					font.pointSize: Config.appearance.fontSize.md
					font.weight: 600

					elide: Text.ElideRight

					anchors {
						left: parent.left
						right: closeButton.left
						verticalCenter: parent.verticalCenter
					}
				}

				MouseArea {
					id: closeButton

					readonly property int iconSize: Config.appearance.fontSize.md * 2
					readonly property int iconPadding: 1

					anchors {
						right: parent.right
						verticalCenter: parent.verticalCenter
					}

					implicitWidth: iconSize
					implicitHeight: iconSize

					cursorShape: Qt.PointingHandCursor
					hoverEnabled: true

					onClicked: {
						root.closeNotification();
					}

					Rectangle {
						anchors.fill: parent
						radius: width / 2

						color: closeButton.containsMouse ? Qt.darker(Colors.colors.destructive, 2) : Qt.darker(Colors.colors.destructive, 3)

						border.width: 1
						border.color: Colors.colors.destructive

						Behavior on color {
							CAnim {
								duration: 100
							}
						}
					}

					MaterialIcon {
						id: closeIcon
						text: "close"
						grade: 200

						font.pixelSize: closeButton.iconSize - closeButton.iconPadding * 2
						color: closeButton.containsMouse ? Colors.colors.destructiveHover : Colors.colors.destructive

						anchors.fill: parent

						horizontalAlignment: Text.AlignHCenter
						verticalAlignment: Text.AlignVCenter

						Behavior on color {
							CAnim {
								duration: 100
							}
						}
					}
				}
			}

			StyledText {
				Layout.fillWidth: true

				text: root.summary

				elide: Text.ElideRight
			}
		}
	}
}
