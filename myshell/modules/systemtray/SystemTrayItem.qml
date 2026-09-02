pragma ComponentBehavior: Bound

import MyShellPlugin
import MyShellPlugin.Configs
import org.nightshell.SystemTray
import org.nightshell.DBusMenu
import QtQuick

MouseArea {
	id: root

	required property StatusNotifierItem modelData
	required property bool isActive

	implicitWidth: height

	Image {
		id: trayIcon

		asynchronous: true
		width: parent.width
		height: parent.height

		source: root.modelData.iconUrl
		scale: root.isActive ? 1.2 : 1
	}
}
