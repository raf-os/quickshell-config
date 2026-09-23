import qs.modules
import qs.modules.background

// import org.nightshell.DBusImageProvider
import org.nightshell.IconProvider
import org.nightshell.IpcServer
import Quickshell
import QtQuick

ShellRoot {
	id: root

	Main {}
	// Background {}

	Component.onCompleted: {
		IPCServer.setup();
	}
}
