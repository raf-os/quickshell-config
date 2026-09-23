pragma ComponentBehavior: Bound

import qs.components

import org.nightshell.SystemTray
import org.nightshell.Configs
import QtQuick
import QtQuick.Layouts

Item {
	id: root

	readonly property bool hasItems: SystemTrayQML.items.values.length > 0
	readonly property StatusNotifierItem current: null
	readonly property int padding: 4

	implicitWidth: trayContents.implicitWidth
	Layout.fillHeight: true

	ListView {
		id: trayContents

		acceptedButtons: Qt.NoButton
		orientation: ListView.Horizontal

		implicitWidth: contentWidth

		anchors {
			top: parent.top
			bottom: parent.bottom
			right: parent.right
		}

		model: SystemTrayQML.items
		delegate: SystemTrayItem {
			id: sysTrayItem
			isActive: root.current === modelData

			implicitHeight: ListView.view ? ListView.view.height : 0
			padding: root.padding
		}
	}
}
