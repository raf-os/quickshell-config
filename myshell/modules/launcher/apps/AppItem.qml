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

	implicitWidth: ListView.view ? ListView.view.width : 0
	implicitHeight: mainLayout.implicitHeight + padding * 2

	function activate() {
		modelData.execute();
	}

	RowLayout {
		id: mainLayout
		spacing: Config.appearance.spacing.md

		anchors {
			left: parent.left
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

				source: `image://qicons/qt/${root.modelData.icon}`
				asynchronous: true
			}
		}

		ColumnLayout {
			id: infoLayout
			spacing: 0

			Layout.fillWidth: true
			Layout.fillHeight: true

			StyledText {
				id: appName
				Layout.fillWidth: true

				text: root.modelData.name
				elide: Text.ElideRight

				font.pointSize: Config.appearance.fontSize.sm
				font.weight: 700
			}

			StyledText {
				id: appDescription
				Layout.fillWidth: true

				text: root.modelData.comment === "" ? "No description provided." : root.modelData.comment
				elide: Text.ElideRight

				font.pointSize: Config.appearance.fontSize.sm
				color: Qt.alpha(Colors.colors.baseContent, 0.5)
			}
		}
	}
}
