pragma ComponentBehavior: Bound

import qs.components

import org.nightshell.Wayland

import QtQuick

LockSurface {
	id: root

	required property bool isActive
	required property LockAuth lockAuth

	property alias inputTextContent: foreground.inputTextContent

	signal unlockRequested
	signal pwTextChanged

	MouseArea {
		anchors.fill: parent

		// WARNING: Only here for testing purposes
		onClicked: root.unlockRequested()

		Rectangle {
			id: bgRect
			anchors.fill: parent
			color: "black"

			opacity: 0

			states: State {
				name: "active"
				when: root.isActive
				PropertyChanges {
					bgRect.opacity: 1
				}
			}

			transitions: [
				Transition {
					to: "active"
					NAnim {
						property: "opacity"
						duration: 400
					}
				},
				Transition {
					to: ""
					NAnim {
						property: "opacity"
						duration: 400
					}
				}
			]
		}

		LockForeground {
			id: foreground
			opacity: bgRect.opacity

			lockAuth: root.lockAuth

			onPwTextChanged: {
				root.pwTextChanged();
			}

			onInputAccepted: {
				root.lockAuth.start();
			}
		}
	}
}
