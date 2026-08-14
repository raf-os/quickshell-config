pragma ComponentBehavior: Bound

import qs.services
import QtQuick
import Quickshell
import Quickshell.Wayland

Variants {
	id: root

	model: Quickshell.screens

	Scope {
		id: scope

		required property ShellScreen modelData

		PanelWindow {
			id: win
			screen: scope.modelData
			color: "transparent"

			readonly property bool isActive: GlobalShellState.isCommandCenterOpen
			readonly property bool isCurrent: GlobalShellState.commandCenterScreen === screen

			anchors {
				right: true
				bottom: true
				left: true
				top: true
			}

			visible: isActive
			exclusionMode: ExclusionMode.Ignore

			WlrLayershell.namespace: "nightshell-commandcenter-overlay"
			WlrLayershell.layer: WlrLayer.Overlay
			WlrLayershell.keyboardFocus: win.isCurrent ? WlrKeyboardFocus.OnDemand : WlrKeyboardFocus.None

			ToplevelView {
				isCurrent: win.isCurrent
			}
		}
	}
}
