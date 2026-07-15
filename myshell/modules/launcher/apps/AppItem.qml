import qs.components
import org.nightshell.DesktopEntries
import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick
import QtQuick.Layouts

MouseArea {
	id: root

	required property DesktopEntry modelData
	required property int index

	readonly property int padding: Config.appearance.padding.sm
	readonly property bool isSelected: ListView.isCurrentItem
	readonly property bool isFavorite: Config.launcher.favoriteApps.includes(modelData.id)

	readonly property color textColor: isSelected ? Colors.colors.primaryContent : Colors.colors.baseContent

	implicitWidth: ListView.view ? ListView.view.width : 0
	implicitHeight: mainLayout.implicitHeight + padding * 2
	cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

	function activate() {
		modelData.execute();
	}

	MouseArea {
		id: favoriteWrapper

		property color iconColor: Qt.alpha(Colors.colors.baseContent, 0.5)

		anchors {
			left: parent.left
			top: parent.top
			bottom: parent.bottom
		}

		implicitWidth: 32
		hoverEnabled: true
		cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

		onClicked: ev => {
			ev.accepted = true;
			EntryManager.toggleFavorite(root.modelData);
		}

		Text {
			anchors.fill: parent
			text: root.isFavorite ? "󰓎" : "󰓒"

			font.family: Config.appearance.fontFamily.monoIcon
			font.pixelSize: parent.height * 0.75

			color: favoriteWrapper.iconColor

			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
		}

		states: [
			State {
				name: "SelectedFavorited"
				when: root.isFavorite === true && root.isSelected === true
				PropertyChanges {
					favoriteWrapper.iconColor: Colors.colors.primaryContent
				}
			},
			State {
				name: "Favorited"
				when: root.isFavorite === true
				PropertyChanges {
					favoriteWrapper.iconColor: Colors.colors.primary
				}
			},
			State {
				name: "Hovered"
				when: favoriteWrapper.containsMouse
				PropertyChanges {
					favoriteWrapper.iconColor: Colors.colors.baseContent
				}
			}
		]
	}

	RowLayout {
		id: mainLayout
		spacing: Config.appearance.spacing.md

		anchors {
			left: favoriteWrapper.right
			leftMargin: root.padding
			right: parent.right
			rightMargin: root.padding
			verticalCenter: parent.verticalCenter
		}

		Item {
			id: iconContainer

			implicitWidth: 24
			implicitHeight: 24

			Layout.alignment: Qt.AlignVCenter

			Image {
				anchors.fill: parent

				width: parent.implicitWidth
				height: parent.implicitHeight

				source: root.modelData.icon ? `image://qicons/qt/${root.modelData.icon}` : ``
				asynchronous: true
			}
		}

		ColumnLayout {
			id: infoLayout
			spacing: 0

			Layout.fillWidth: true
			Layout.fillHeight: true

			AppItemText {
				id: appName

				text: root.modelData.name
				color: root.textColor

				font.weight: 700
			}

			AppItemText {
				id: appDescription

				text: root.modelData.comment === "" ? "No description provided." : root.modelData.comment
				elide: Text.ElideRight
				color: Qt.alpha(root.textColor, 0.5)
			}
		}
	}

	component AppItemText: StyledText {
		Layout.fillWidth: true

		elide: Text.ElideRight
		font.pointSize: Config.appearance.fontSize.sm
		font.family: Config.appearance.fontFamily.sans
	}
}
