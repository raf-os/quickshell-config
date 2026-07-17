pragma ComponentBehavior: Bound

import org.nightshell.Hyprland
import QtQuick

import ".." as Launcher

Launcher.BaseList {
	id: root
	view: lView

	function selectItem() {
		if (lView.currentItem as WindowItem) {
			(lView.currentItem as WindowItem).activate();
		}
		requestClose();
	}

	Component.onCompleted: {
		Hyprland.toplevelModel.searchQuery = root.queryString;
	}

	onQueryStringChanged: {
		Hyprland.toplevelModel.searchQuery = root.queryString;
	}

	Launcher.BaseListView {
		id: lView
		anchors.fill: parent

		model: Hyprland.toplevelModel.items
		delegate: WindowItem {
			onClicked: {
				modelData.activate();
				root.requestClose();
			}
		}
	}
}
