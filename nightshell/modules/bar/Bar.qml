import qs.components

import Quickshell
import QtQuick

Item {
	id: root

	required property BasePanelWindow panelWindow
	required property ShellScreen screen

	implicitHeight: 32

	Rectangle {
		anchors.fill: parent
	}
}
