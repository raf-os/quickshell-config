pragma ComponentBehavior: Bound

import "popouts"
import MyShellPlugin.Configs
import Quickshell
import QtQuick

Item {
	id: root

	property alias content: content

	required property PanelWindow rootPanel
	required property ShellScreen screen
	required property PersistentProperties openPanels
	required property PopoutWrapper popouts
	required property Item panels

	readonly property int paddingH: Config.appearance.padding.xl
	readonly property int paddingV: Config.appearance.padding.sm

	readonly property int contentHeight: Config.bar.sizes.innerHeight + paddingV * 2
	readonly property int exclusiveZone: contentHeight

	readonly property int barRelativeX: content.x
	readonly property int barRelativeY: content.y

	readonly property int systemTrayRelativeX: content.systemTrayX

	readonly property Item mediaInfo: content.mediaInfo
	implicitHeight: contentHeight

	function handleMouseWheel(x: real, y: real, angleDelta: point) {
		content.handleMouseWheel(x, y, angleDelta);
	}

	Bar {
		id: content

		anchors.fill: parent

		anchors.topMargin: root.paddingV
		anchors.bottomMargin: root.paddingV
		anchors.leftMargin: root.paddingH
		anchors.rightMargin: root.paddingH

		// implicitHeight: Config.bar.sizes.innerHeight
		screen: root.screen

		openPanels: root.openPanels
		popouts: root.popouts
		panels: root.panels
		rootPanel: root.rootPanel
	}
}
