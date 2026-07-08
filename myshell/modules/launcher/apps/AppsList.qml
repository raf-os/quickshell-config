pragma ComponentBehavior: Bound

import qs.components
import org.nightshell.DesktopEntries
import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick

Item {
	id: root

	property string queryString

	DesktopEntriesModel {
		id: entriesModel
		queryString: root.queryString
	}

	signal requestClose

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

	SListView {
		id: view
		z: 0

		anchors.fill: parent
		clip: true
		spacing: Config.appearance.spacing.sm

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
		listView.highlightMoveDuration: 150

		highlight: Rectangle {
			color: Colors.colors.primary
		}
	}
}
