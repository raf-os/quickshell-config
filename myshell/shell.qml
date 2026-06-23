import qs.modules
import qs.modules.background as WLRBG
import qs.modules.lock
import qs.services
import org.nightshell.Notifications
import MyShellPlugin
import MyShellPlugin.Configs
import Quickshell
import QtQuick

ShellRoot {
	id: root

	WLRBG.Background {}
	Drawers {}
	HyprShortcuts {}
	Lock {}

	Connections {
		target: Config.notification

		function onEnabledChanged() {
			root.evalNotificationConfig();
		}
	}

	function evalNotificationConfig() {
		NotificationServer.isActive = Config.notification.enabled;
	}

	Component.onCompleted: {
		UserCommandService.setup();
		SystemSoundsService.setup();

		root.evalNotificationConfig();

		// Config.earlyLoad();
		// Colors.earlyLoad();
	}
}
