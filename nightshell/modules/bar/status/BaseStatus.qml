pragma ComponentBehavior: Bound
import qs.modules.bar

import QtQuick

BaseWidget {
	id: root

	property string icon

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

	Loader {
		active: root.icon != ""
		anchors.fill: parent

		sourceComponent: Item {
			anchors.fill: parent
			anchors.margins: 2
			Image {
				asynchronous: true
				anchors.centerIn: parent
				width: parent.width
				height: parent.height
				source: `image://qicons/shell/${root.icon}`
			}
		}
	}
}
