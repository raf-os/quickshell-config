import qs.modules.bar.tray

import QtQuick
import QtQuick.Layouts

Item {
	id: root

	RowLayout {
		id: mainLayout

		anchors.fill: parent

		Item {
			Layout.fillWidth: true
			Layout.fillHeight: true
		}

		SystemTray {}
		TimeWidget {}
	}
}
