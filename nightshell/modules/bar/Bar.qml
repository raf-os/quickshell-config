pragma ComponentBehavior: Bound

import qs.components
import qs.modules.bar.workspaces
import qs.modules.bar.left as BARLEFT

import org.nightshell.Hyprland
import org.nightshell.Configs as C
import Quickshell
import QtQuick

Item {
	id: root

	required property BasePanelWindow panelWindow
	required property ShellScreen screen
	readonly property HyprMonitor nsMonitor: Hyprland.monitorsModel.values.find(m => m.name === screen.name) ?? null
	readonly property ToplevelInstance activeToplevel: Hyprland.toplevelModel.activeToplevel

	readonly property int leftLength: leftComponent.width
	readonly property int centerLength: workspacesItems.width

	readonly property int padding: 4

	property alias leftComponent: leftComponent

	implicitHeight: 32

	BARLEFT.Main {
		id: leftComponent

		activeToplevel: root.activeToplevel

		anchors {
			left: parent.left
			leftMargin: root.padding
			right: centerComponent.left
			rightMargin: centerComponent.height
			top: parent.top
			topMargin: root.padding
			bottom: parent.bottom
			bottomMargin: root.padding
		}
	}

	Item {
		id: centerComponent

		anchors {
			top: parent.top
			topMargin: root.padding
			bottom: parent.bottom
			bottomMargin: root.padding
			horizontalCenter: parent.horizontalCenter
		}

		implicitWidth: workspacesItems.implicitWidth

		WorkspacesGuide {
			id: workspacesItems
			monitorName: root.screen.name

			anchors {
				top: parent.top
				bottom: parent.bottom
				horizontalCenter: parent.horizontalCenter
			}
			workspaceDelegate: WorkspaceItem {
				size: parent.height
				isActive: root.nsMonitor.activeWorkspace === this.modelData
			}
		}
	}
}
