pragma ComponentBehavior: Bound

import qs.components
import qs.modules.launcher.apps

import org.nightshell.Components
import org.nightshell.Configs
import org.nightshell.DesktopEntries
import org.nightshell.Utils

import QtQuick
import QtQuick.Effects

MouseArea {
	id: root

	required property bool isActive
	required property int maxWidth
	required property int maxHeight

	readonly property int animDuration: 200
	readonly property int hPadding: 200
	readonly property int vPadding: 100

	anchors.centerIn: parent

	implicitWidth: maxWidth - hPadding * 2
	implicitHeight: maxHeight - vPadding * 2

	signal closeRequested
	signal exitAnimationFinished

	Component.onCompleted: {
		queryInput.forceActiveFocus();
	}

	onIsActiveChanged: {
		if (isActive) {
			queryInput.forceActiveFocus();
		}
	}

	Keys.onPressed: ev => {
		let shouldAccept = true;
		switch (ev.key) {
		case Qt.Key_Escape:
			root.closeRequested();
			break;
		case Qt.Key_Tab:
			appList.moveForwards();
			break;
		case Qt.Key_Backtab:
			appList.moveBackwards();
			break;
		case Qt.Key_Return:
			appList.selectCurrent();
			break;
		default:
			shouldAccept = false;
			break;
		}

		ev.accepted = shouldAccept;
	}

	DesktopEntriesModel {
		id: entriesModel
		queryString: queryInput.text
	}

	// FocusGrabber {
	// 	active: root.isActive
	// 	onFocusLost: root.closeRequested()
	// }

	RectangularShadow {
		id: bgShadow
		anchors.centerIn: parent
		color: "black"
		blur: 32
		opacity: 0
		implicitWidth: root.maxWidth
		implicitHeight: parent.implicitHeight + Styles.padding_xl * 2
	}

	Item {
		id: wrapper
		anchors.fill: parent

		scale: 0.9
		opacity: 0

		states: State {
			when: root.isActive
			name: "active"

			PropertyChanges {
				bgShadow.opacity: 0.5
				wrapper.scale: 1
				wrapper.opacity: 1
			}
		}

		transitions: [
			Transition {
				to: "active"
				NAnim {
					properties: "scale,opacity"
					duration: root.animDuration
				}
			},
			Transition {
				to: ""
				SequentialAnimation {
					NAnim {
						properties: "scale, opacity"
						duration: root.animDuration
					}
					ScriptAction {
						script: root.exitAnimationFinished()
					}
				}
			}
		]

		AppsList {
			id: appList
			model: entriesModel

			anchors {
				top: parent.top
				bottom: searchBar.top
				bottomMargin: Styles.spacing_lg
				left: parent.left
				right: parent.right
			}

			onRequestClose: {
				root.closeRequested();
			}
		}

		Item {
			id: searchBar

			anchors {
				left: parent.left
				right: parent.right
				bottom: parent.bottom
			}

			implicitHeight: 24 + 8

			ChamferRect {
				anchors.fill: parent
				color: Colors.secondaryContent
				bottomRightChamfer: Math.round(height / 3)
				borderWidth: 1
				borderColor: Colors.secondary
			}

			ShellIcon {
				id: searchIcon

				anchors {
					left: parent.left
					leftMargin: 4
					verticalCenter: parent.verticalCenter
				}
				icon: "search"
				size: 24
				colorize: true
				color: Colors.secondary
				glow: true
			}

			TextInput {
				id: queryInput

				focus: true

				anchors.fill: parent
				padding: Styles.padding_md
				leftPadding: searchIcon.size + Styles.spacing_md

				font.family: Config.appearance.fontFamily.sans
				font.letterSpacing: 1
				font.pointSize: Styles.text_md

				selectionColor: Colors.secondary
				selectedTextColor: Colors.secondaryContent

				color: Colors.secondary

				cursorDelegate: Rectangle {
					implicitWidth: 2
					color: queryInput.color
					visible: queryInput.activeFocus
				}

				Keys.forwardTo: [root]
			}

			SimpleGlowEffect {
				source: queryInput
				anchors.fill: queryInput
				shadowColor: queryInput.color
			}
		}
	}
}
