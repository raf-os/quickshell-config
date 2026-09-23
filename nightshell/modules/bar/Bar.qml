pragma ComponentBehavior: Bound

import qs.components
import qs.modules.bar.workspaces
import qs.modules.bar.left as BARLEFT
import qs.modules.bar.right as BARRIGHT

import org.nightshell.Hyprland
import org.nightshell.Configs
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
	readonly property int paddingH: Styles.padding_md
	readonly property int spacing: Styles.spacing_lg

	property alias leftComponent: leftComponent

	implicitHeight: 36

	BARLEFT.Main {
		id: leftComponent

		activeToplevel: root.activeToplevel

		anchors {
			left: parent.left
			leftMargin: root.paddingH
			right: centerComponent.left
			rightMargin: root.spacing
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

	BARRIGHT.Main {
		id: rightComponent

		anchors {
			top: parent.top
			topMargin: root.padding
			bottom: parent.bottom
			bottomMargin: root.padding
			left: centerComponent.right
			leftMargin: root.spacing
			right: parent.right
			rightMargin: root.paddingH
		}
	}
}
