pragma ComponentBehavior: Bound

import "popouts"
import MyShellPlugin.Configs
import Quickshell
import QtQuick

Item {
	id: root

	required property ShellScreen screen
	required property PersistentProperties openPanels
	required property PopoutWrapper popouts
	required property Item panels

	readonly property int paddingH: Config.appearance.padding.xl
	readonly property int paddingV: Config.appearance.padding.sm

	readonly property int contentHeight: Config.bar.sizes.innerHeight + paddingV * 2
	readonly property int exclusiveZone: contentHeight

	readonly property Item mediaInfo: content.item ? content.item?.mediaInfo : null
	implicitHeight: contentHeight

	function handleMouseWheel(x: real, y: real, angleDelta: point) {
		content.item?.handleMouseWheel(x, y, angleDelta);
	}

	Loader {
		id: content

		anchors.fill: parent

		sourceComponent: Bar {
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
		}
	}
}
