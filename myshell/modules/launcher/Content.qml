import qs.components
import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick
import QtQuick.Effects

import "apps"

MouseArea {
	id: root

	required property real boundsWidth
	required property real boundsHeight
	readonly property int borderPadding: 200

	implicitWidth: boundsWidth - borderPadding * 2
	implicitHeight: boundsHeight - borderPadding * 2

	focus: true

	Component.onCompleted: {
		queryInput.forceActiveFocus();
	}

	onClicked: {
		queryInput.forceActiveFocus();
	}

	signal closeLauncherRequested
	signal nextItemRequested
	signal previousItemRequested
	signal currentItemActivated

	Keys.onReturnPressed: ev => {
		root.currentItemActivated();
	}

	Keys.onEscapePressed: ev => {
		ev.accepted = true;
		root.closeLauncherRequested();
	}

	Keys.onTabPressed: ev => {
		ev.accepted = true;
		root.nextItemRequested();
	}
	Keys.onDownPressed: ev => {
		ev.accepted = true;
		root.nextItemRequested();
	}

	Keys.onUpPressed: ev => {
		ev.accepted = true;
		root.previousItemRequested();
	}
	Keys.onBacktabPressed: ev => {
		ev.accepted = true;
		root.previousItemRequested();
	}

	RectangularShadow {
		anchors.fill: parent

		color: "black"
	}

	Rectangle {
		id: bgRect

		anchors.fill: parent

		color: Colors.colors.base
		radius: Config.appearance.rounding.sm

		border.width: root.activeFocus ? 2 : 0
		border.color: root.activeFocus ? Colors.colors.primary : "transparent"
	}

	Item {
		id: content

		anchors.fill: parent
		anchors.margins: Config.appearance.padding.sm

		AppsList {
			id: appList
			queryString: queryInput.text

			onRequestClose: {
				root.closeLauncherRequested();
			}

			anchors {
				top: parent.top
				left: parent.left
				right: parent.right
				bottom: inputWrapper.top
				bottomMargin: content.anchors.bottomMargin
			}

			Connections {
				target: root

				function onNextItemRequested() {
					appList.moveListForwards();
				}

				function onPreviousItemRequested() {
					appList.moveListBackwards();
				}

				function onCurrentItemActivated() {
					appList.selectItem();
				}
			}
		}

		Item {
			id: inputWrapper

			readonly property int textPadding: Config.appearance.padding.sm

			anchors {
				left: parent.left
				right: parent.right
				bottom: parent.bottom
			}

			implicitHeight: queryInput.implicitHeight

			Rectangle {
				id: bgInput
				anchors.fill: parent
				color: Colors.colors.base0
				radius: Config.appearance.rounding.sm

				border.width: queryInput.activeFocus ? 2 : 0
				border.color: Colors.colors.primary
			}

			MaterialIcon {
				id: searchIcon

				anchors {
					left: parent.left
					top: parent.top
					bottom: parent.bottom
				}

				font.pixelSize: parent.implicitHeight * 0.75
				text: "search"

				leftPadding: 4
				rightPadding: 4

				verticalAlignment: Text.AlignVCenter
			}

			Text {
				id: placeholderText

				anchors.fill: parent
				font: queryInput.font
				padding: inputWrapper.textPadding
				leftPadding: searchIcon.width + Config.appearance.spacing.xxs

				text: "Search apps..."
				color: Qt.alpha(Colors.colors.baseContent, 0.5)

				opacity: queryInput.text === "" ? 1 : 0

				Behavior on opacity {
					NumberAnimation {
						duration: 150
					}
				}
			}

			TextInput {
				id: queryInput

				focus: true
				anchors.fill: parent
				padding: inputWrapper.textPadding
				leftPadding: searchIcon.width + Config.appearance.spacing.xxs

				font.family: Config.appearance.fontFamily.mono
				font.pointSize: Config.appearance.fontSize.sm

				color: Colors.colors.baseContent
			}
		}
	}
}
