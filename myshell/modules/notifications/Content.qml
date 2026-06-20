pragma ComponentBehavior: Bound

import qs.utils
import qs.components
import org.nightshell.Notifications
import MyShellPlugin
import MyShellPlugin.Configs
import Quickshell
import Quickshell.Hyprland
import QtQuick
import QtQuick.Layouts

MouseArea {
	id: root

	required property bool isActive
	required property real desiredWidth
	required property OpenPanels openPanels

	readonly property int padding: Config.appearance.padding.md
	readonly property int leftPadding: padding
	readonly property int rightPadding: Math.max(0, padding - Config.border.thickness)

	implicitWidth: 0
	clip: true

	focus: isActive

	onClicked: {
		root.resetExpansion();
	}
	propagateComposedEvents: true

	anchors {
		top: parent.top
		bottom: parent.bottom
		right: parent.right
	}

	signal readyToUnload
	signal lostFocus

	Component.onCompleted: {
		const sId = root.openPanels.extractSelectedNotificationId();
		if (sId !== -1) {
			lview.currentIndex = sId;
		}
	}

	function resetExpansion(): void {
		lview.currentIndex = -1;
	}

	function expandId(idx: int): bool {
		if (lview.currentIndex === idx) {
			resetExpansion();
			return false;
		}
		lview.currentIndex = idx;
		return true;
	}

	onIsActiveChanged: {
		if (isActive) {
			root.forceActiveFocus();
		}
	}

	Keys.onEscapePressed: {
		root.resetExpansion();
		root.lostFocus();
	}

	HyprlandFocusGrab {
		active: root.isActive
		windows: [QsWindow.window]
		onCleared: {
			root.resetExpansion();
			root.isActive = false;
		}
	}

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
				// target: root
				property: "implicitWidth"
				duration: 200
			}
		},
		Transition {
			to: ""
			SequentialAnimation {
				NAnim {
					// target: root
					property: "implicitWidth"
					duration: 200
				}

				ScriptAction {
					script: root.readyToUnload()
				}
			}
		}
	]

	Item {
		id: header

		anchors {
			top: parent.top
			left: parent.left
		}

		implicitWidth: root.desiredWidth
		implicitHeight: headerLayout.implicitHeight + root.padding * 2

		RowLayout {
			id: headerLayout

			anchors {
				left: parent.left
				leftMargin: root.leftPadding
				right: parent.right
				rightMargin: root.rightPadding
				verticalCenter: parent.verticalCenter
			}

			StyledText {
				text: "Notifications"
				Layout.fillWidth: true

				font.family: Config.appearance.fontFamily.sans
				font.pointSize: Config.appearance.fontSize.lg
				font.weight: 600
			}

			HeaderButton {
				text: "Clear"

				onClicked: {
					NotificationServer.closeAllNotifications();
					root.isActive = false;
				}
			}
		}
	}

	SListView {
		id: lview
		model: NotificationServer.model

		spacing: Config.appearance.spacing.sm

		anchors {
			top: header.bottom
			left: parent.left
			leftMargin: root.leftPadding
			bottom: parent.bottom
		}

		listView.highlightFollowsCurrentItem: true
		listView.highlightMoveDuration: -1
		listView.highlightMoveVelocity: -1

		onScrollBarPressed: {
			root.resetExpansion();
		}

		implicitWidth: root.desiredWidth - root.leftPadding - root.rightPadding
		clip: true

		delegate: NotificationItem {
			id: notifDelegate

			expandedId: ListView.view ? ListView.view.currentIndex : -1

			onRequestExpand: idx => {
				if (root.expandId(idx)) {}
			}

			onCloseNotification: {
				if (!modelData)
					return;
				modelData.dismiss();
			}

			background: Rectangle {
				readonly property color bgCol: notifDelegate.urgency === NotificationUrgency.Critical ? Colors.colors.destructive : Colors.colors.base2
				anchors.fill: parent
				color: notifDelegate.containsMouse ? Qt.lighter(bgCol, 1.1) : bgCol
				radius: Config.appearance.rounding.sm

				border.width: notifDelegate.isExpanded ? 2 : 0
				border.color: Colors.colors.primary

				Behavior on color {
					CAnim {
						duration: 100
					}
				}
			}

			bodyContent: Item {
				id: notifBody

				anchors {
					left: parent.left
					right: parent.right
					top: parent.top
				}
				clip: true

				implicitHeight: notifBodyLayout.height

				ColumnLayout {
					id: notifBodyLayout

					anchors {
						left: parent.left
						right: parent.right
						top: parent.top
					}

					StyledText {
						Layout.fillWidth: true
						text: notifDelegate.body !== "" ? notifDelegate.body : notifDelegate.summary

						font.family: Config.appearance.fontFamily.sans
						font.pointSize: Config.appearance.fontSize.sm

						wrapMode: Text.Wrap
						maximumLineCount: notifDelegate.isExpanded ? 10000 : 2
						elide: Text.ElideRight
					}

					Item {
						Layout.fillWidth: true

						implicitHeight: bodyActionsLayout.height
						clip: true

						ColumnLayout {
							id: bodyActionsLayout

							anchors {
								top: parent.top
								left: parent.left
								right: parent.right
							}

							Repeater {
								Layout.fillWidth: true

								model: notifDelegate.notificationActions

								delegate: ActionButton {
									required property NotificationAction modelData

									Layout.fillWidth: true
									hasIcon: notifDelegate.hasActionIcons

									onInvoked: {
										modelData.invoke();
									}
								}
							}
						}
					}
				}
			}
		}
	}

	Item {
		id: noNotifItem

		anchors.fill: lview
		opacity: NotificationServer.model.values.length === 0 ? 1 : 0 // qmllint disable unresolved-type

		StyledText {
			anchors.fill: parent

			text: "No new notifications."

			font.pointSize: Config.appearance.fontSize.sm
		}

		Behavior on opacity {
			NAnim {
				duration: 200
			}
		}
	}

	component HeaderButton: MouseArea {
		id: headerBtn
		property string text: ""
		property int padding: Config.appearance.padding.sm
		property color color: Colors.colors.base2

		implicitWidth: btnText.width
		implicitHeight: btnText.height

		hoverEnabled: true
		cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

		function onClicked() {
		}

		onClicked: {
			if (enabled)
				onClicked();
		}

		Rectangle {
			id: headerBtnBg
			anchors.fill: parent
			radius: Config.appearance.rounding.sm

			color: headerBtn.color

			border.width: 1
			border.color: Qt.darker(headerBtn.color, 2)

			Behavior on color {
				enabled: GlobalStateManager.isGameMode === false
				CAnim {
					duration: 100
				}
			}

			states: [
				State {
					when: headerBtn.enabled === false
					PropertyChanges {
						headerBtnBg.color: Colors.colors.base0
					}
				},
				State {
					when: headerBtn.pressed
					PropertyChanges {
						headerBtnBg.color: Qt.darker(headerBtn.color, 2)
					}
				},
				State {
					when: headerBtn.containsMouse
					PropertyChanges {
						headerBtnBg.color: Qt.lighter(headerBtn.color, 1.25)
					}
				}
			]
		}

		Text {
			id: btnText

			anchors.centerIn: parent

			text: headerBtn.text
			font.family: Config.appearance.fontFamily.sans
			font.pointSize: Config.appearance.fontSize.xxs
			font.weight: 600

			padding: headerBtn.padding

			color: Colors.colors.baseContent
		}
	}

	component ActionButton: MouseArea {
		id: actionButtonComponent

		required property string text
		required property string identifier
		required property bool hasIcon

		readonly property int padding: 4

		implicitHeight: Config.appearance.fontSize.xl + padding * 2
		cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
		hoverEnabled: true

		signal invoked

		onClicked: {
			invoked();
		}

		Rectangle {
			id: actionButtonBg
			anchors.fill: parent

			color: Colors.colors.base

			border.width: 1
			border.color: actionButtonComponent.containsMouse ? Colors.colors.primary : Colors.colors.neutral

			radius: Config.appearance.rounding.sm
		}

		RowLayout {
			id: actionButtonLayout

			anchors.fill: parent
			anchors.margins: actionButtonComponent.padding

			Loader {
				active: actionButtonComponent.hasIcon

				Layout.fillHeight: true

				sourceComponent: Image {
					id: actionButtonIcon

					asynchronous: true
					anchors.verticalCenter: parent.verticalCenter

					width: actionButtonLayout.implicitHeight
					height: width

					source: `image://qicons/qt/${actionButtonComponent.identifier}`
				}
			}

			Text {
				id: actionButtonText

				Layout.fillWidth: true
				Layout.fillHeight: true

				text: actionButtonComponent.text
				elide: Text.ElideRight

				font.family: Config.appearance.fontFamily.sans
				font.pixelSize: parent.height * 0.75

				color: Colors.colors.baseContent

				verticalAlignment: Text.AlignVCenter
			}
		}
	}
}
