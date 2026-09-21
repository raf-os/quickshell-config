import org.nightshell.Hyprland as NSH
import org.nightshell.Configs
import QtQuick
import QtQuick.Layouts

RowLayout {
	id: root

	required property Component workspaceDelegate
	required property string monitorName
	readonly property int workspaceIconSize: 24

	spacing: Styles.spacing_sm
	implicitWidth: 300

	Repeater {
		model: NSH.Hyprland.workspacesModel.values.filter(w => w.monitorName === root.monitorName)
		delegate: root.workspaceDelegate
	}
}
