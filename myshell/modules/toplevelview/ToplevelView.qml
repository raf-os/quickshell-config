pragma ComponentBehavior: Bound

import qs.services
import MyShellPlugin
import MyShellPlugin.Configs
import org.nightshell.Hyprland as NSHypr
import org.nightshell.Wayland
import QtQuick
import QtQuick.Layouts
import Quickshell
import Quickshell.Wayland

MouseArea {
	id: root

	required property bool isCurrent

	anchors.fill: parent
	focus: isCurrent

	Keys.onEscapePressed: ev => {
		ev.accepted = true;
		GlobalShellState.closeCommandCenter();
	}

	Keys.onReleased: ev => {
		if (ev.key === Qt.Key_Meta) {
			// todo
		}
	}

	onClicked: ev => {
		ev.accepted = true;
		GlobalShellState.closeCommandCenter();
	}

	Rectangle {
		anchors.fill: parent
		color: "black"
		opacity: 0.8
	}

	Loader {
		anchors.fill: parent
		active: root.isCurrent

		sourceComponent: Item {
			anchors.fill: parent
			anchors.margins: 200

			Rectangle {
				anchors.fill: parent
				color: Colors.colors.base
			}

			GridView {
				id: winGrid

				acceptedButtons: Qt.NoButton

				cellWidth: 240
				cellHeight: 200

				readonly property int maxCol: Math.floor(parent.width / cellWidth)
				readonly property int rowNum: Math.max(Math.ceil(count / maxCol), 1)

				implicitWidth: Math.min(count * cellWidth, maxCol * cellWidth)
				implicitHeight: Math.min(rowNum * cellHeight, parent.width)

				readonly property bool isClipped: contentHeight > parent.height

				anchors {
					horizontalCenter: parent.horizontalCenter
					verticalCenter: parent.verticalCenter
				}

				clip: true
				focus: true

				model: NSHypr.Hyprland.toplevelModel.items

				delegate: ToplevelItem {
					implicitWidth: winGrid.cellWidth
					implicitHeight: winGrid.cellHeight
				}

				highlight: Rectangle {
					color: Colors.colors.primary
					radius: Config.appearance.rounding.sm
				}
			}
		}
	}
}
