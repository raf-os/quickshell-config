import qs.components
import org.nightshell.Configs
import org.nightshell.Hyprland

import QtQuick

Item {
	id: root

	required property ToplevelInstance activeToplevel
	readonly property int titleX: titleWrapper.x
	readonly property int titleWidth: titleWrapper.width

	property alias osIcon: osIconWrapper

	Item {
		id: osIconWrapper

		readonly property int padding: 4

		anchors {
			top: parent.top
			left: parent.left
			leftMargin: Styles.spacing_sm
			bottom: parent.bottom
		}

		implicitWidth: osIcon.width + padding * 4

		OSIcon {
			id: osIcon
			anchors {
				top: parent.top
				bottom: parent.bottom
				horizontalCenter: parent.horizontalCenter
				margins: osIconWrapper.padding
			}
		}
	}

	Item {
		id: titleWrapper
		readonly property int hPadding: Styles.padding_md
		implicitWidth: Math.min(toplevelTitle.width + hPadding * 2, root.width - x)

		anchors {
			top: parent.top
			bottom: parent.bottom
			left: osIconWrapper.right
			leftMargin: Styles.spacing_sm
		}

		clip: true

		TextMetrics {
			id: titleMetrics
			text: root.activeToplevel ? root.activeToplevel.title : "Desktop"

			font.family: Config.appearance.fontFamily.sans
			font.pixelSize: (titleWrapper.height * 0.5)
			font.weight: 600
			font.capitalization: Font.AllUppercase

			elide: Text.ElideRight
			elideWidth: root.width - titleWrapper.x - titleWrapper.hPadding
		}

		StyledText {
			id: toplevelTitle

			anchors {
				left: parent.left
				leftMargin: titleWrapper.hPadding
				top: parent.top
				bottom: parent.bottom
			}

			width: Math.min(titleMetrics.width + titleWrapper.hPadding, titleMetrics.elideWidth)

			text: titleMetrics.elidedText
			color: Colors.secondaryContent

			elide: Text.ElideRight

			font: titleMetrics.font

			verticalAlignment: Text.AlignVCenter
		}
	}
}
