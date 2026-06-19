pragma Singleton

import org.nightshell.Notifications
import MyShellPlugin.Configs
import Quickshell
import QtQuick

Singleton {
	id: root

	property alias model: temporaryNotifications

	function clearNotifications(): void {
		temporaryNotifications.clear();
	}

	function addNotification(notif: Notification): void {
		temporaryNotifications.insert(0, {
			modelData: notif
		});
		startCullTimer();
	}

	function removeNotificationById(idx: int): void {
		if (temporaryNotifications.get(idx))
			temporaryNotifications.remove(idx, 1);
	}

	function startCullTimer(): void {
		cullTimer.restart();
	}

	function stopCullTimer(): void {
		cullTimer.stop();
	}

	Connections {
		target: NotificationServer
		enabled: Config.notification.enabled

		function onNotification(notification: Notification): void {
			root.addNotification(notification);
		}
	}

	Timer {
		id: cullTimer
		interval: 5000
		onTriggered: {
			temporaryNotifications.clear();
		}
	}

	ListModel {
		id: temporaryNotifications
	}
}
