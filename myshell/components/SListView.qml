import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick
import QtQuick.Controls

Item {
	id: root

	property alias listView: listView

	required property var model
	property Component delegate: null
	property Component highlight: null
	property Component background: null

	property int spacing
	property int padding

	property alias currentItem: listView.currentItem
	property alias currentIndex: listView.currentIndex
	property alias count: listView.count

	signal scrollBarPressed

	Loader {
		anchors.fill: listView
		anchors.margins: -root.padding

		active: root.background !== null
		sourceComponent: root.background
	}

	ListView {
		id: listView

		acceptedButtons: Qt.NoButton

		readonly property bool isScrollBarActive: contentHeight > height
		readonly property int scrollBarWidth: 8
		readonly property int scrollBarClearance: isScrollBarActive ? scrollBarWidth + Config.appearance.spacing.sm : 0

		anchors.fill: parent
		anchors.margins: root.padding
		anchors.rightMargin: root.padding + scrollBarClearance

		clip: true
		boundsBehavior: Flickable.StopAtBounds
		spacing: root.spacing

		model: root.model
		delegate: root.delegate
		highlight: root.highlight

		ScrollBar.vertical: ScrollBar {
			parent: root

			policy: listView.isScrollBarActive ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff

			onPressedChanged: {
				if (pressed)
					root.scrollBarPressed();
			}

			anchors.top: parent.top
			anchors.right: parent.right
			anchors.bottom: parent.bottom

			implicitWidth: listView.scrollBarWidth

			padding: 0

			contentItem: Rectangle {
				color: Colors.colors.primary
				radius: width / 2
			}

			background: Rectangle {
				color: Colors.colors.base2
				radius: width / 2
			}
		}
	}
}
