import qs.components

import org.nightshell.Components
import org.nightshell.Configs
import org.nightshell.Utils

import QtQuick
import QtQuick.Effects

MouseArea {
	id: root

	required property bool isActive

	property color bgCol: isActive ? Colors.accent : Colors.secondary
	property color fgCol: isActive ? Colors.accentContent : Colors.secondaryContent
	readonly property int animDuration: 300

	implicitWidth: height + Styles.padding_md * 2
	cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

	Behavior on bgCol {
		CAnim {
			duration: root.animDuration
		}
	}

	ChamferRect {
		id: bgRect
		anchors.fill: parent
		visible: false
		color: root.bgCol
		bottomLeftChamfer: 8
	}

	SimpleGlowEffect {
		source: bgRect
		anchors.fill: bgRect
		shadowColor: root.bgCol
	}

	XDGIcon {
		id: iconImage
		anchors.centerIn: parent
		asynchronous: true
		size: 18
		icon: SysInfo.iconName
	}

	WrappedMultiEffect {
		source: iconImage
		anchors.fill: iconImage
		colorization: 1
		colorizationColor: root.fgCol
		brightness: 0.5
		scale: root.isActive ? 1.2 : 1

		Behavior on scale {
			NAnim {
				duration: root.animDuration
			}
		}
	}
}
