import org.nightshell.Configs

import QtQuick
import QtQuick.Controls

Item {
	id: root

	property alias view: listView

	required property var model
	property Component delegate: null
	property Component highlight: null

	property int spacing
	property int padding
	property int scrollBarWidth: 8
	property int scrollBarSpacing: Styles.spacing_md

	property alias currentItem: listView.currentItem
	property alias currentIndex: listView.currentIndex
	property alias count: listView.count

	property alias highlightFollowsCurrentItem: listView.highlightFollowsCurrentItem
	property alias highlightMoveDuration: listView.highlightMoveDuration
	property alias highlightMoveVelocity: listView.highlightMoveVelocity
	property alias highlightResizeDuration: listView.highlightResizeDuration
	property alias highlightResizeVelocity: listView.highlightResizeVelocity

	property alias keyNavigationEnabled: listView.keyNavigationEnabled
	property alias keyNavigationWraps: listView.keyNavigationWraps

	signal scrollBarPressed

	ListView {
		id: listView

		acceptedButtons: Qt.NoButton

		readonly property bool isScrollBarActive: contentHeight > height
		readonly property int scrollBarClearance: isScrollBarActive ? root.scrollBarWidth + root.scrollBarSpacing : 0

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
			// Visual parent: won't get clipped by listView's
			// clip property despite being outside its bounds
			parent: root

			policy: listView.isScrollBarActive ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff

			onPressedChanged: {
				if (pressed)
					root.scrollBarPressed();
			}

			anchors.top: parent.top
			anchors.right: parent.right
			anchors.bottom: parent.bottom

			implicitWidth: root.scrollBarWidth

			padding: 0

			contentItem: Rectangle {
				color: Colors.primary
			}

			background: Rectangle {
				color: Colors.primaryContent
			}
		}
	}
}
