import QtQuick

Item {
	id: root
	required property string icon

	Image {
		asynchronous: true
		anchors.centerIn: parent
		width: parent.width
		height: parent.height
		source: `image://qicons/shell/${root.icon}`
	}
}
