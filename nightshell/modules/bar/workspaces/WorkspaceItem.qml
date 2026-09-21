import org.nightshell.Hyprland
import org.nightshell.Configs
import QtQuick

Item {
	id: root

	required property int size
	required property HyprWorkspace modelData
	required property bool isActive

	readonly property int borderSpacing: 3
	readonly property bool hasToplevels: modelData.toplevels.length !== 0

	implicitWidth: size
	implicitHeight: size

	Rectangle {
		anchors.centerIn: parent
		implicitWidth: root.hasToplevels ? (root.size - root.borderSpacing * 2) : 8
		implicitHeight: implicitWidth
		radius: width / 2

		color: root.isActive ? Colors.primary : Colors.primaryContent
	}
}
