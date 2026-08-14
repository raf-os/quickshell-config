import org.nightshell.Wayland
import org.nightshell.Hyprland
import QtQuick

MouseArea {
	id: root

	required property ToplevelInstance modelData

	cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

	Rectangle {
		anchors.fill: parent
		anchors.margins: 8
	}

	ScreencopyQMLView {
		captureSource: root.modelData.waylandHandle
		constraints: Qt.size(90, 0)
	}
}
