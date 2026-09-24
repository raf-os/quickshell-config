import org.nightshell.Configs
import org.nightshell.SystemTray
import QtQuick
import QtQuick.Effects

MouseArea {
	id: root

	required property StatusNotifierItem modelData
	required property bool isActive
	property int padding: 4

	acceptedButtons: Qt.AllButtons
	hoverEnabled: true
	cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

	implicitWidth: implicitHeight

	signal openAttachedMenu
	signal requestClose

	onClicked: ev => {
		if (ev.button === Qt.LeftButton) {
			if (root.modelData.isMenuOnly) {
				root.openAttachedMenu();
			} else {
				root.modelData.activate();
			}
		} else if (ev.button === Qt.MiddleButton) {
			if (root.modelData.isMenuOnly)
				return;
			root.modelData.secondaryActivate();
		} else if (ev.button === Qt.RightButton) {
			root.openAttachedMenu();
		}
	}

	onModelDataChanged: {
		if (!modelData)
			root.requestClose();
	}

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
