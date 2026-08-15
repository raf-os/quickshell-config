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

	signal moveSelectionForward
	signal moveSelectionBackward
	signal moveSelectionUp
	signal moveSelectionDown
	signal selectCurrentItem

	Connections {
		target: GlobalShellState

		function onCommandCenterForward() {
			root.moveSelectionForward();
		}
	}

	Keys.onEscapePressed: ev => {
		ev.accepted = true;
		GlobalShellState.closeCommandCenter();
	}

	Keys.onReleased: ev => {
		switch (ev.key) {
		case Qt.Key_Meta:
			root.selectCurrentItem();
			return;
		default:
			ev.accepted = false;
			return;
		}
	}

	Keys.onPressed: ev => {
		switch (ev.key) {
		case Qt.Key_Tab:
		case Qt.Key_Right:
			root.moveSelectionForward();
			return;
		case Qt.Key_Backtab:
		case Qt.Key_Left:
			root.moveSelectionBackward();
			return;
		default:
			ev.accepted = false;
			return;
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
				cellHeight: 160

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
				keyNavigationWraps: true

				model: NSHypr.Hyprland.toplevelModel.items

				Component.onCompleted: {
					if (count > 1) {
						currentIndex = 1;
					}
				}

				delegate: ToplevelItem {
					implicitWidth: winGrid.cellWidth
					implicitHeight: winGrid.cellHeight

					onCloseRequested: {
						GlobalShellState.closeCommandCenter();
					}
				}

				highlightMoveDuration: 100
				highlight: Rectangle {
					color: Colors.colors.primary
					radius: Config.appearance.rounding.sm
				}

				Connections {
					target: root

					function onMoveSelectionForward() {
						winGrid.moveCurrentIndexRight();
					}

					function onMoveSelectionBackward() {
						winGrid.moveCurrentIndexLeft();
					}

					function onMoveSelectionUp() {
						winGrid.moveCurrentIndexUp();
					}

					function onMoveSelectionDown() {
						winGrid.moveCurrentIndexDown();
					}

					function onSelectCurrentItem() {
						const item = (winGrid.currentItem as ToplevelItem);
						if (item) {
							item.select();
						}
					}
				}
			}
		}
	}
}
