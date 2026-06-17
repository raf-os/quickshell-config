import qs.utils
import qs.components
import org.nightshell.Notifications
import MyShellPlugin
import MyShellPlugin.Configs
import Quickshell
import Quickshell.Hyprland
import QtQuick
import QtQuick.Layouts

Item {
	id: root

	required property bool isActive
	required property real desiredWidth

	implicitWidth: 0
	clip: true

	anchors {
		top: parent.top
		bottom: parent.bottom
		right: parent.right
	}

	signal readyToUnload
	signal lostFocus

	HyprlandFocusGrab {
		active: root.isActive
		windows: [QsWindow.window]
		onCleared: {
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

		readonly property int padding: Config.appearance.padding.md

		anchors {
			top: parent.top
			left: parent.left
		}

		implicitWidth: root.desiredWidth
		implicitHeight: headerLayout.implicitHeight + padding * 2

		RowLayout {
			id: headerLayout

			anchors {
				left: parent.left
				leftMargin: header.padding
				right: parent.right
				rightMargin: header.padding
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
				}
			}
		}
	}

	SListView {
		id: listView
		model: NotificationServer.model

		anchors {
			top: header.bottom
			left: parent.left
			bottom: parent.bottom
		}
		implicitWidth: root.desiredWidth
		clip: true

		delegate: NotificationItem {
			onCloseNotification: {
				modelData.dismiss();
			}
		}
	}

	Item {
		id: noNotifItem

		anchors.fill: listView
		anchors.margins: Config.appearance.padding.md

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
}
