import qs.services

import Quickshell
import QtQuick

PersistentProperties {
	id: root
	reloadableId: "persistentOpenPanelState"

	required property ShellScreen screen

	property bool session
	property bool startmenu
	property bool launcher
	property bool commandCapture
	property bool mprisViewer
	property bool notifications

	property string desiredStartMenuTab: ""
	property int _selectedNotificationBuffer: -1

	Component.onCompleted: PanelService.load(screen, this)

	function openNotificationsAtId(idx: int): void {
		root.notifications = true;
		root._selectedNotificationBuffer = Math.max(-1, idx);
	}

	function extractSelectedNotificationId(): int {
		const cur = root._selectedNotificationBuffer;
		root._selectedNotificationBuffer = -1;
		return cur;
	}

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
