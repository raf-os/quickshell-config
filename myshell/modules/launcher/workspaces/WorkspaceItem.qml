pragma ComponentBehavior: Bound

import qs.components
import org.nightshell.Hyprland
import org.nightshell.DesktopEntries
import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick
import QtQuick.Layouts

MouseArea {
	id: root

	required property HyprWorkspace modelData

	readonly property bool isSelected: ListView.isCurrentItem
	readonly property bool hasName: modelData.name !== ""

	implicitWidth: ListView.view ? ListView.view.width : 0
	implicitHeight: 32

	cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

	function activate() {
		Hyprland.dispatch(`hl.dsp.focus({ workspace = "${modelData.id}" })`);
	}

	StyledText {
		text: `[${root.modelData.id}] ${root.hasName ? root.modelData.name : `Unnamed workspace`}`
		anchors.fill: parent

		font.family: Config.appearance.fontFamily.sans
		font.pointSize: Config.appearance.fontSize.md

		padding: 4

		verticalAlignment: Text.AlignVCenter
	}

	RowLayout {
		id: windowsLayout

		anchors {
			top: parent.top
			bottom: parent.bottom
			right: parent.right
		}

		Repeater {
			model: root.modelData.toplevels

			delegate: Item {
				id: toplevelItem
				required property ToplevelInstance modelData
				implicitWidth: 24
				implicitHeight: implicitWidth

				Rectangle {
					anchors.fill: parent
					color: Colors.colors.base2
					radius: Config.appearance.rounding.sm
				}

				Image {
					anchors.centerIn: parent
					asynchronous: true
					width: 16
					height: 16
					source: `image://qicons/qt/${EntryManager.findEntry(toplevelItem.modelData.appId).id}?fallback=application-octet-stream`
				}
			}
		}
	}
}
