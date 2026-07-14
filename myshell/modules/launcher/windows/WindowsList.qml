import org.nightshell.Hyprland.Toplevels
import QtQuick

import ".." as Launcher

Launcher.BaseList {
	id: root
	view: lView

	Launcher.BaseListView {
		id: lView
		anchors.fill: parent

		// model: []
		model: ToplevelManager.toplevels
		delegate: WindowItem {}
	}
}
