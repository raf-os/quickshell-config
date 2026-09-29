pragma ComponentBehavior: Bound

import qs.components

import org.nightshell.Components
import org.nightshell.Hyprland
import org.nightshell.Configs

import QtQuick
import QtQuick.Effects

MouseArea {
	id: root

	required property int size
	required property HyprWorkspace modelData
	required property bool isActive

	readonly property int borderSpacing: 3
	readonly property bool hasToplevels: modelData.toplevels.length !== 0

	implicitWidth: size
	implicitHeight: size

	cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

	onClicked: {
		Hyprland.dispatch(`hl.dsp.focus({ workspace = "${root.modelData.id}" })`);
	}

	RectangularShadow {
		anchors.fill: parent
		opacity: root.isActive ? 0.75 : 0
		blur: 10
		spread: 2
		color: Colors.secondary
	}

	ChamferRect {
		anchors {
			fill: parent
		}

		bottomRightChamfer: 6

		color: root.isActive ? Colors.secondary : Colors.primaryContent
		borderWidth: 1
		borderColor: root.isActive ? Colors.secondary : Colors.primary
	}

	Loader {
		id: workspaceIconLoader
		active: root.hasToplevels
		anchors.centerIn: parent
		sourceComponent: Item {
			anchors.centerIn: parent

			implicitWidth: Styles.text_xl
			implicitHeight: implicitWidth

			Image {
				id: workspaceIconImage
				visible: false
				width: parent.implicitWidth
				height: parent.implicitHeight
				source: `image://qicons/shell/${root.modelData.toplevels.length <= 1 ? "window" : "window_multiple"}`
			}

			MultiEffect {
				source: workspaceIconImage
				anchors.fill: parent
				colorization: 1
				colorizationColor: root.isActive ? Colors.secondaryContent : Colors.primary
			}
		}
	}
}
