import "cards"

import qs.components
import qs.utils

import org.nightshell.Configs
import org.nightshell.Components

import QtQuick
import QtQuick.Controls

Item {
	id: root

	required property InstanceContext context
	required property bool isActive
	required property int type

	readonly property int desiredWidth: 320
	readonly property int animDuration: 300

	readonly property int padding: 20

	implicitWidth: desiredWidth / 2
	implicitHeight: 96
	opacity: 0

	signal requestClose
	signal exitAnimationFinished

	function typeToComponent(type: int): Component {
		switch (type) {
		case OSD.Type.GameMode:
			return gamemodeCard;
		default:
			return null;
		}
	}

	onTypeChanged: {
		if (root.type >= OSD.Type.MAX)
			return;

		const newComp = typeToComponent(root.type);
		if (newComp) {
			view.replaceCurrentItem(newComp, {}, StackView.Immediate);
		}
	}

	states: State {
		name: "active"
		when: root.isActive

		PropertyChanges {
			root.opacity: 1
			root.implicitWidth: root.desiredWidth
		}
	}

	transitions: [
		Transition {
			to: "active"
			NAnim {
				properties: "implicitWidth,opacity"
				duration: root.animDuration
			}
		},
		Transition {
			to: ""
			SequentialAnimation {
				NAnim {
					properties: "implicitWidth,opacity"
					duration: root.animDuration
				}
				ScriptAction {
					script: root.exitAnimationFinished()
				}
			}
		}
	]

	ChamferRect {
		anchors.fill: view
		anchors.margins: -root.padding * 0.5
		color: Qt.alpha(Colors.neutral, 0.75)
		borderWidth: 2
		borderColor: Colors.primary

		topLeftChamfer: 12
	}

	StackView {
		id: view

		anchors {
			left: parent.left
			top: parent.top
			bottom: parent.bottom
			margins: root.padding
		}

		implicitWidth: root.desiredWidth - root.padding * 2
		clip: true
	}

	Component {
		id: gamemodeCard
		GamemodeCard {}
	}
}
