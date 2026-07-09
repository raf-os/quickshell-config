pragma ComponentBehavior: Bound

import qs.components
import org.nightshell.DesktopEntries
import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick
import QtQuick.Layouts

Item {
	id: root

	property string queryString
	property bool highlightHotkeys

	DesktopEntriesModel {
		id: entriesModel
		queryString: root.queryString
	}

	signal requestClose
	signal hotkeyTriggered(key: int)

	function moveListForwards() {
		view.listView.incrementCurrentIndex();
	}

	function moveListBackwards() {
		view.listView.decrementCurrentIndex();
	}

	function selectItem() {
		if (view.currentItem as AppItem) {
			(view.currentItem as AppItem).activate();
		}
		requestClose();
	}

	Item {
		id: emptyListDisplay

		anchors.fill: parent

		visible: view.count === 0

		StyledText {
			anchors.fill: parent
			text: "No apps found for current query"

			color: Qt.alpha(Colors.colors.baseContent, 0.5)

			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
		}
	}

	SListView {
		id: view

		background: Rectangle {
			anchors.fill: parent
			color: Colors.colors.base0
			radius: Config.appearance.rounding.sm
		}

		anchors {
			left: parent.left
			right: parent.right
			top: parent.top
			bottom: filtersWrapper.top
		}

		clip: true
		spacing: Config.appearance.spacing.sm
		padding: 4

		listView.keyNavigationWraps: true

		model: entriesModel.entryList
		delegate: AppItem {
			id: appItemDelegate

			onClicked: {
				modelData.execute();
				root.requestClose();
			}
		}

		listView.highlightFollowsCurrentItem: true
		listView.highlightResizeVelocity: -1
		listView.highlightResizeDuration: -1
		listView.highlightMoveVelocity: -1
		listView.highlightMoveDuration: -1

		highlight: Rectangle {
			color: Colors.colors.primary
			radius: Config.appearance.rounding.sm - view.padding
		}
	}

	Item {
		id: filtersWrapper

		anchors {
			left: parent.left
			right: parent.right
			bottom: parent.bottom
		}

		implicitHeight: 32

		RowLayout {
			anchors.fill: parent
			anchors.leftMargin: Config.appearance.padding.sm
			anchors.rightMargin: anchors.leftMargin

			FilterItem {
				name: "(ALT+1) Hide terminal-only entries"
				hotKey: Qt.Key_1
				isActive: entriesModel.hideTerminalOnly

				function trigger() {
					entriesModel.hideTerminalOnly = !entriesModel.hideTerminalOnly;
				}
			}
		}
	}

	component FilterItem: MouseArea {
		id: filterItemRoot

		hoverEnabled: true
		cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

		readonly property int spacing: Config.appearance.spacing.sm
		required property string name
		property int hotKey

		property bool isActive

		Layout.fillHeight: true

		implicitWidth: filterCheckbox.width + spacing + filterLabel.width

		onClicked: {
			trigger();
		}

		function trigger() {
		}

		Connections {
			target: root
			enabled: filterItemRoot.hotKey > 0

			function onHotkeyTriggered(key: int) {
				if (key === filterItemRoot.hotKey)
					filterItemRoot.trigger();
			}
		}

		Rectangle {
			id: filterCheckbox

			anchors {
				left: parent.left
				verticalCenter: parent.verticalCenter
			}

			implicitWidth: Config.appearance.fontSize.md
			implicitHeight: implicitWidth

			radius: Config.appearance.rounding.xs
			color: filterItemRoot.isActive ? Colors.colors.primary : Colors.colors.neutral

			border.width: 1
			border.color: filterItemRoot.containsMouse || root.highlightHotkeys ? Qt.lighter(Colors.colors.primary, 1.25) : Qt.alpha("black", 0.5)
		}

		StyledText {
			id: filterLabel

			anchors {
				left: filterCheckbox.right
				leftMargin: filterItemRoot.spacing
				top: parent.top
				bottom: parent.bottom
			}

			text: `${filterItemRoot.name}`
			verticalAlignment: Text.AlignVCenter
			font.pointSize: Config.appearance.fontSize.xs
		}
	}
}
