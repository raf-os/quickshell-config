import qs.modules as Modules
import qs.modules.background

import org.nightshell.DBusImageProvider
import org.nightshell.IconProvider
import org.nightshell.IpcServer
import Quickshell
import QtQuick

ShellRoot {
	id: root

	Modules.Main {}
	// Background {}

	Component.onCompleted: {
		// Singletons are lazily loaded, so here the necessary ones are loaded by force
		// (don't worry they enjoy it)
		IPCServer.setup();
	}
}
