import qs.modules.bar.tray
import qs.modules.bar.status as STATUS

import QtQuick
import QtQuick.Layouts

Item {
	id: root

	property alias systemTray: systemTray
	property alias statusItems: statusItems

	RowLayout {
		id: mainLayout

		anchors.fill: parent

		Item {
			Layout.fillWidth: true
			Layout.fillHeight: true
		}

		STATUS.Main {
			id: statusItems
		}
		SystemTray {
			id: systemTray
		}
		TimeWidget {}
	}
}
