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

	Launcher.BaseListView {
		id: lView
		anchors.fill: parent

		model: Hyprland.toplevelModel
		delegate: WindowItem {
			onClicked: {
				modelData.activate();
				root.requestClose();
			}
		}
	}
}
