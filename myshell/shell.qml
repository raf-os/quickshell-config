import qs.modules
import qs.modules.background as WLRBG
import qs.modules.toplevelview
import qs.modules.lock
import qs.services
import org.nightshell.IpcServer
import org.nightshell.DBusImageProvider
import org.nightshell.IconProvider
import org.nightshell.SystemTray
import org.nightshell.Notifications
import org.nightshell.DesktopEntries as NDE
import MyShellPlugin
import MyShellPlugin.Configs
import Quickshell
import QtQuick

ShellRoot {
	id: root

	WLRBG.Background {}
	Drawers {}
	CommandCenterWrapper {}
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
		// UserCommandService.setup();
		SystemSoundsService.setup();
		IPCServer.setup();
		NDE.EntryManager.init();

		root.evalNotificationConfig();

		// Config.earlyLoad();
		// Colors.earlyLoad();
	}
}
