import qs.components

import org.nightshell.Configs
import org.nightshell.Utils

import QtQuick

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

	scale: 0.8
	opacity: 0

	focus: true

	signal closeRequested

	FocusGrabber {
		active: true
		onFocusLost: root.closeRequested()
	}

	states: State {
		when: root.isActive
		name: "active"

		PropertyChanges {
			root.scale: 1
			root.opacity: 1
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
					script: root.closeRequested()
				}
			}
		}
	]

	Rectangle {
		anchors.fill: parent
	}
}
