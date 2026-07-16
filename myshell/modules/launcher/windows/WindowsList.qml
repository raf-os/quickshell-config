import org.nightshell.Hyprland
import QtQuick

import ".." as Launcher

Launcher.BaseList {
	id: root
	view: lView

	Launcher.BaseListView {
		id: lView
		anchors.fill: parent

		model: Hyprland.toplevelModel
		delegate: WindowItem {}
	}
}
