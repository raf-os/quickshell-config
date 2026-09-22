pragma ComponentBehavior: Bound

import qs.components

import org.nightshell.Hyprland
import org.nightshell.Configs
import QtQuick
import QtQuick.Effects

Item {
	id: root

	required property int size
	required property HyprWorkspace modelData
	required property bool isActive

	readonly property int borderSpacing: 3
	readonly property bool hasToplevels: modelData.toplevels.length !== 0

	implicitWidth: size
	implicitHeight: size

	RectangularShadow {
		anchors.fill: parent
		opacity: root.isActive ? 0.75 : 0
		blur: 10
		spread: 2
		color: Colors.secondary
	}

	Rectangle {
		anchors {
			fill: parent
		}

		color: root.isActive ? Colors.secondary : Colors.primaryContent
		border.width: 1
		border.color: root.isActive ? Colors.secondary : Colors.primary
	}

	Loader {
		id: workspaceIconLoader
		active: root.hasToplevels
		anchors.fill: parent
		sourceComponent: Item {
			anchors.fill: parent
			anchors.margins: 2

			Image {
				id: workspaceIconImage
				visible: false
				width: parent.width
				height: parent.height
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
