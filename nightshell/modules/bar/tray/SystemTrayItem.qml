import org.nightshell.Configs
import org.nightshell.SystemTray
import QtQuick
import QtQuick.Effects

MouseArea {
	id: root

	required property StatusNotifierItem modelData
	required property bool isActive
	property int padding: 4

	hoverEnabled: true
	cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

	implicitWidth: implicitHeight

	Image {
		id: trayItemIcon
		visible: false
		anchors.centerIn: parent

		asynchronous: true
		width: parent.width - root.padding * 2
		height: parent.height - root.padding * 2

		source: root.modelData.iconUrl
	}

	MultiEffect {
		source: trayItemIcon
		anchors.fill: trayItemIcon
		colorization: 0.5
		colorizationColor: Colors.secondary
		contrast: -0.1
		brightness: 0.1
	}
}
