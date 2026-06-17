import qs.services

import Quickshell
import QtQuick

PersistentProperties {
	id: root
	reloadableId: "persistentOpenPanelState"

	required property ShellScreen screen

	property bool session
	property bool startmenu
	property bool commandCapture
	property bool mprisViewer
	property bool notifications

	property string desiredStartMenuTab: ""

	Component.onCompleted: PanelService.load(screen, this)

	function closePanels() {
		session = false;
		startmenu = false;
		mprisViewer = false;
		notifications = false;
	}

	function toggleNotifications() {
		if (notifications) {
			notifications = false;
			return;
		}

		session = false;
		startmenu = false;
		mprisViewer = false;

		notifications = true;
	}
}
