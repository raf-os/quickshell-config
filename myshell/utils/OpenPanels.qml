import qs.services

import Quickshell
import QtQuick

PersistentProperties {
	id: root

	required property ShellScreen screen

	property bool session
	property bool startmenu
	property bool commandCapture
	property bool mprisViewer

	property string desiredStartMenuTab: ""

	Component.onCompleted: PanelService.load(screen, this)
}
