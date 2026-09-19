import qs.components

import Quickshell
import QtQuick

Item {
	id: root

	required property BasePanelWindow panelWindow
	required property ShellScreen screen

	readonly property int topLineHeight: 4

	implicitHeight: 32

	Rectangle {
		anchors.fill: parent
		color: "transparent"
	}
}
