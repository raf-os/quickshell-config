import qs.components
import org.nightshell.Hyprland.Toplevels
import QtQuick

MouseArea {
	id: root

	required property ToplevelHandle modelData

	implicitWidth: ListView.view ? ListView.view.width : 0
	implicitHeight: 24

	cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

	StyledText {
		id: toplevelText
		text: `${root.modelData.title} | ${root.modelData.appId}`

		anchors.verticalCenter: parent.verticalCenter
	}
}
