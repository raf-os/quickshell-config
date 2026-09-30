import qs.components

import org.nightshell.Components
import org.nightshell.DesktopEntries
import org.nightshell.Configs

import QtQuick
import QtQuick.Layouts

MouseArea {
	id: root

	required property DesktopEntry modelData
	required property int index

	readonly property bool hasDescription: modelData.comment !== ""

	readonly property bool isSelected: ListView.isCurrentItem
	readonly property bool isFavorite: false
	readonly property int padding: Styles.padding_sm
	readonly property int spacing: Styles.spacing_xl

	readonly property color baseColor: isSelected ? Colors.accent : Colors.primaryContent
	readonly property color altColor: isSelected ? Colors.accent : Colors.primary
	readonly property color fgColor: isSelected ? Colors.accentContent : Colors.neutralContent

	implicitWidth: ListView.view ? ListView.view.width : 0
	implicitHeight: Math.max(mainLayout.implicitHeight, favoriteWrapper.height) + (padding * 2)

	cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

	signal selected

	function select() {
		root.modelData.execute();
		root.selected();
	}

	onClicked: select()

	ChamferRect {
		anchors.fill: parent
		color: root.baseColor
		borderWidth: 1
		borderColor: root.altColor

		topLeftChamfer: 12
		bottomRightChamfer: 12
	}

	MouseArea {
		id: favoriteWrapper

		anchors {
			left: parent.left
			top: parent.top
			bottom: parent.bottom
		}

		implicitWidth: 24 + root.spacing * 2
		hoverEnabled: true
		cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

		ShellIcon {
			id: favoriteIcon
			anchors.centerIn: parent
			icon: root.isFavorite ? "star_filled" : "star_hollow"
			size: 24
		}

		Rectangle {
			anchors {
				top: parent.top
				topMargin: 1
				bottom: parent.bottom
				bottomMargin: 1
				right: parent.right
			}

			implicitWidth: 1
			color: root.isSelected ? root.fgColor : root.altColor
		}
	}

	RowLayout {
		id: mainLayout
		spacing: Styles.spacing_xl

		anchors {
			left: favoriteWrapper.right
			leftMargin: root.spacing
			right: parent.right
			verticalCenter: parent.verticalCenter
		}

		Item {
			id: iconContainer

			Layout.alignment: Qt.AlignVCenter
			implicitWidth: 24
			implicitHeight: 24

			XDGIcon {
				id: xdgIcon
				anchors.fill: parent
				icon: root.modelData.icon ? root.modelData.icon : "application-octet-stream"
				fallback: root.modelData.icon ? "application-octet-stream" : ""
				size: 24
			}
		}

		ColumnLayout {
			id: infoLayout

			Layout.fillWidth: true
			Layout.alignment: Qt.AlignVCenter

			StyledText {
				id: appName

				Layout.fillWidth: true

				text: root.modelData.name
				color: root.fgColor
				font.weight: 600
				font.pointSize: Styles.text_sm
			}

			StyledText {
				id: appDescription

				Layout.fillWidth: true

				text: root.hasDescription ? root.modelData.comment : "No description provided"
				color: root.fgColor
				font.pointSize: Styles.text_xs

				opacity: root.hasDescription ? 0.66 : 0.33
			}
		}
	}
}
