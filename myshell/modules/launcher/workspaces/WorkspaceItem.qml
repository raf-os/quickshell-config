import qs.components
import org.nightshell.Hyprland
import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick

MouseArea {
	id: root

	required property HyprWorkspace modelData

	readonly property bool isSelected: ListView.isCurrentItem
	readonly property bool hasName: modelData.name !== ""

	implicitWidth: ListView.view ? ListView.view.width : 0
	implicitHeight: 32

	cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

	function activate() {
	}

	StyledText {
		text: `[${root.modelData.id}] ${root.hasName ? root.modelData.name : `Unnamed workspace`}`
		anchors.fill: parent

		font.family: Config.appearance.fontFamily.sans
		font.pointSize: Config.appearance.fontSize.md

		padding: 4

		verticalAlignment: Text.AlignVCenter
	}
}
