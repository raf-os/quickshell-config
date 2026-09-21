pragma ComponentBehavior: Bound

import qs.components
import qs.modules.bar.workspaces

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

	readonly property int topLineHeight: 4

	implicitHeight: 32

	Item {
		id: leftComponent

		anchors {
			left: parent.left
			right: centerComponent.left
			rightMargin: centerComponent.height
			top: parent.top
			topMargin: root.topLineHeight
			bottom: parent.bottom
		}

		StyledText {
			id: toplevelTitle

			anchors {
				top: parent.top
				bottom: parent.bottom
				left: parent.left
			}

			text: root.activeToplevel ? root.activeToplevel.title : "Desktop"
			color: C.Colors.secondaryContent
			font.weight: 600
			font.pixelSize: parent.height * 0.5

			verticalAlignment: Text.AlignVCenter
		}
	}

	Item {
		id: centerComponent

		anchors {
			top: parent.top
			bottom: parent.bottom
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
				size: workspacesItems.workspaceIconSize
				isActive: root.nsMonitor.activeWorkspace === this.modelData
			}
		}
	}
}
