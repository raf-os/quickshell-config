import qs.modules.bar.tray
import qs.modules.bar.status as STATUS

import QtQuick
import QtQuick.Layouts

Item {
	id: root

	property alias systemTray: systemTray

	RowLayout {
		id: mainLayout

		anchors.fill: parent

		Item {
			Layout.fillWidth: true
			Layout.fillHeight: true
		}

		STATUS.Main {}
		SystemTray {
			id: systemTray
		}
		TimeWidget {}
	}
}
