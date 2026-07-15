import qs.components
import org.nightshell.Hyprland
import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick

MouseArea {
	id: root

	required property ToplevelInstance modelData

	readonly property bool isSelected: ListView.isCurrentItem
	readonly property color textColor: isSelected ? Colors.colors.primaryContent : Colors.colors.baseContent
	readonly property int padding: Config.appearance.padding.sm
	readonly property int spacing: Config.appearance.spacing.md

	implicitWidth: ListView.view ? ListView.view.width : 0
	implicitHeight: Math.max(imageWrapper.height, toplevelText.height) + padding * 2

	cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

	Item {
		id: imageWrapper

		anchors {
			left: parent.left
			leftMargin: root.padding
			verticalCenter: parent.verticalCenter
		}

		implicitWidth: 24

		Image {
			id: appIcon

			asynchronous: true
			anchors.centerIn: parent
			width: 24
			height: 24
			source: `image://qicons/${root.modelData.appId}`
		}
	}

	StyledText {
		id: toplevelText
		text: `${root.modelData.title}`

		anchors {
			left: imageWrapper.right
			leftMargin: root.spacing
			right: parent.right
			verticalCenter: parent.verticalCenter
		}

		rightPadding: root.padding
		elide: Text.ElideRight
		color: root.textColor

		font.family: Config.appearance.fontFamily.sans
		font.pointSize: Config.appearance.fontSize.sm
	}
}
