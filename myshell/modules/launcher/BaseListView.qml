pragma ComponentBehavior: Bound

import qs.components
import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick

SListView {
	id: root

	background: Rectangle {
		anchors.fill: parent
		color: Colors.colors.base0
		radius: Config.appearance.rounding.sm
	}

	clip: true
	spacing: 0
	padding: 4
	scrollBarWidth: 10

	listView.keyNavigationWraps: true

	listView.highlightFollowsCurrentItem: true
	listView.highlightResizeVelocity: -1
	listView.highlightResizeDuration: -1
	listView.highlightMoveVelocity: -1
	listView.highlightMoveDuration: -1

	highlight: Rectangle {
		color: Colors.colors.primary
		radius: Config.appearance.rounding.sm - root.padding
	}
}
