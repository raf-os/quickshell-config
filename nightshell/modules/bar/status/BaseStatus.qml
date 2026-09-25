pragma ComponentBehavior: Bound
import qs.modules.bar

import QtQuick

BaseWidget {
	id: root

	signal clicked(ev: MouseEvent)

	anchors {
		top: parent.top
		bottom: parent.bottom
	}

	implicitWidth: height

	MouseArea {
		id: mouseArea
		anchors.fill: parent

		onClicked: ev => root.clicked(ev)
		enabled: root.enabled
		cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
	}
}
