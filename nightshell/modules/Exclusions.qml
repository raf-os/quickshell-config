pragma ComponentBehavior: Bound

import qs.components

import Quickshell
import QtQuick

Scope {
	id: root

	required property ShellScreen screen
	required property Item bar

	ExclusionZone {
		id: topExclusion

		anchors.top: true
		exclusiveZone: root.bar.implicitHeight
	}

	component ExclusionZone: BasePanelWindow {
		screen: root.screen
		name: "border-exclusion"
		mask: Region {}
		implicitWidth: 1
		implicitHeight: 1
	}
}
