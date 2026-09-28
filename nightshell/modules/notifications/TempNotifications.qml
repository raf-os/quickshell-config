pragma Singleton

import org.nightshell.Notifications
import org.nightshell.Configs

import Quickshell
import QtQuick

Singleton {
	id: root

	property alias model: model

	function clear(): void {
		model.clear();
	}

	function addNotification(notification: Notification): void {
		model.insert(0, {
			modelData: notification
		});
		startCullTimer();
	}

	function removeNotification(idx: int): void {
		if (model.get(idx))
			model.remove(idx, 1);
	}

	function startCullTimer(): void {
		cullTimer.restart();
	}

	function stopCullTimer(): void {
		cullTimer.stop();
	}

	Connections {
		target: NotificationServer
		enabled: Config.modules.enabledModules.notifications

		function onNotification(notification: Notification): void {
			root.addNotification(notification);
		}
	}

	Timer {
		id: cullTimer
		interval: 5000
		onTriggered: {
			root.clear();
		}
	}

	ListModel {
		id: model
	}
}
