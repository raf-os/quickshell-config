pragma ComponentBehavior: Bound

import qs.components

import org.nightshell.SystemTray
import org.nightshell.Configs
import QtQuick
import QtQuick.Layouts

Item {
	id: root

	readonly property bool hasItems: SystemTrayQML.items.values.length > 0
	property SystemTrayItem current: null
	readonly property int padding: 4

	implicitWidth: trayContents.implicitWidth
	Layout.fillHeight: true

	function toggleItem(item: SystemTrayItem): void {
		activationCooldown.running = true;
		if (root.current === item) {
			root.current = null;
		} else {
			root.current = item;
		}
	}

	function closePopup() {
		root.current = null;
	}

	Timer {
		id: activationCooldown
		interval: 500
	}

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
			isActive: root.current === this

			implicitHeight: ListView.view ? ListView.view.height : 0
			padding: root.padding

			onOpenAttachedMenu: {
				if (activationCooldown.running)
					return;
				root.toggleItem(sysTrayItem);
			}
		}
	}
}
