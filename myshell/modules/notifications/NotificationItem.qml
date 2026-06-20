pragma ComponentBehavior: Bound

import qs.components
import org.nightshell.Notifications
import MyShellPlugin
import MyShellPlugin.Configs
import Quickshell
import QtQml
import QtQuick
import QtQuick.Layouts

MouseArea {
	id: root

	required property int index // list ID
	required property Notification modelData
	property int enterExitDuration: 300
	property int padding: Config.appearance.padding.md
	property Component background: DefaultBackground {}
	property Component bodyContent: null
	property bool isTemporary: false
	property int expandedId: -1
	readonly property bool isExpanded: expandedId === index

	property int notifIdx
	property int urgency
	property string appName
	property string appIcon
	property string summary
	property string body
	property string imageUrl
	property bool hasActionIcons
	property list<NotificationAction> notificationActions

	Binding {
		when: (root.modelData !== null && root.modelData !== undefined) && root.isTemporary === false
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

	function modelRefetch() {
		if (!modelData || !root.isTemporary)
			return;

		notifIdx = modelData.id;
		urgency = modelData.urgency;
		appName = modelData.appName;
		appIcon = modelData.appIcon;
		summary = modelData.summary;
		imageUrl = modelData.imageUrl;
	}

	onModelDataChanged: {
		root.modelRefetch();
	}

	Connections {
		target: root.modelData
		enabled: root.modelData

		function onClosed(reason: int): void {
			root.forceClose();
		}
	}

	readonly property int desiredWidth: ListView.view ? ListView.view.width : 0

	implicitWidth: desiredWidth
	implicitHeight: mainLayout.implicitHeight + padding * 2

	hoverEnabled: root.isTemporary === false

	signal closeNotification
	signal forceClose
	signal requestExpand(idx: int)
	signal activated(idx: int)

	onClicked: ev => {
		if (root.isTemporary) {
			root.activated(root.index);
		} else {
			root.requestExpand(root.index);
		}
	}

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

	Loader {
		active: true
		anchors.fill: parent

		sourceComponent: root.background
	}

	component DefaultBackground: Rectangle {
		id: backgroundRect

		anchors.fill: parent
		color: Colors.colors.base
		radius: Config.appearance.rounding.sm

		border.width: root.urgency === NotificationUrgency.Critical ? 2 : 0
		border.color: Colors.colors.destructive
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

		spacing: Config.appearance.spacing.md

		Item {
			id: iconArea

			readonly property int size: 48
			readonly property string appName: root.appName
			readonly property DesktopEntry desktopEntry: appName === "" ? null : DesktopEntries.byId(appName)
			readonly property string appIcon: root.appIcon === "" ? desktopEntry ? desktopEntry.icon : "" : root.appIcon

			implicitWidth: size
			implicitHeight: size

			Layout.alignment: Qt.AlignTop

			Loader {
				active: (iconArea.desktopEntry || iconArea.appIcon !== "") && root.imageUrl === ""
				anchors.fill: parent

				sourceComponent: Image {
					source: Quickshell.iconPath(iconArea.appIcon)

					width: iconArea.size
					height: iconArea.size

					anchors.fill: parent
				}
			}

			Loader {
				active: root.imageUrl === "" && iconArea.appIcon === ""
				anchors.fill: parent

				sourceComponent: Item {
					anchors.fill: parent

					Rectangle {
						anchors.fill: parent

						radius: width / 2
						color: Colors.colors.base3

						border.width: 2
						border.color: Colors.colors.base4
					}

					MaterialIcon {
						anchors.fill: parent

						text: "question_mark"
						color: Colors.colors.base4
						font.pixelSize: height * 0.8

						verticalAlignment: Text.AlignVCenter
						horizontalAlignment: Text.AlignHCenter
					}
				}
			}
		}

		ColumnLayout {
			id: layoutTextContent
			Layout.fillWidth: true

			spacing: Config.appearance.spacing.xxs
			clip: true

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

						color: closeButton.containsMouse ? Colors.colors.destructive : Colors.colors.base

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
						color: closeButton.containsMouse ? Colors.colors.destructiveContent : Colors.colors.destructive

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

			Loader {
				active: root.bodyContent !== null

				Layout.fillWidth: true

				sourceComponent: root.bodyContent
			}
		}
	}
}
