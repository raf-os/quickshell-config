pragma ComponentBehavior: Bound

import qs.components
import org.nightshell.Hyprland
import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick

import "../" as Launcher

Launcher.BaseList {
	id: root
	view: lview

	tabIdx: Launcher.Tabs.T.Workspaces

	function selectItem() {
		if (view.currentItem as WorkspaceItem) {
			(view.currentItem as WorkspaceItem).activate();
		}
		requestClose();
	}

	Launcher.BaseListView {
		id: lview
		anchors.fill: parent

		model: Hyprland.workspacesModel.values
		delegate: WorkspaceItem {}

		listView.section.property: "monitorId"
		listView.section.criteria: ViewSection.FullString
		listView.section.delegate: Item {
			id: monitorSection
			required property string section
			readonly property HyprMonitor monitor: {
				const idx = parseInt(section);
				const found = Hyprland.monitorsModel.values.find(mon => mon.id === idx);
				if (found)
					return found;
				return null;
			}
			implicitWidth: ListView.view ? ListView.view.width : 0
			implicitHeight: monitorLabel.height + underline.height + Config.appearance.spacing.sm

			Rectangle {
				id: underline

				anchors {
					left: parent.left
					right: parent.right
					bottom: parent.bottom
					bottomMargin: Config.appearance.spacing.sm
				}
				implicitHeight: 2

				color: Colors.colors.base2
			}

			StyledText {
				id: monitorLabel

				anchors {
					left: parent.left
					right: parent.right
					top: parent.top
				}

				text: monitorSection.monitor?.name ?? "Unknown monitor"
				font.family: Config.appearance.fontFamily.sans
				font.pointSize: Config.appearance.fontSize.sm
				font.weight: 700
				elide: Text.ElideRight

				padding: Config.appearance.padding.sm
			}
		}
	}
}
