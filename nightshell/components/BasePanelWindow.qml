import Quickshell
import Quickshell.Wayland
import QtQuick

PanelWindow {
	id: root

	required property string name
	WlrLayershell.namespace: `nightshell-${root.name}`
	color: "transparent"
}
