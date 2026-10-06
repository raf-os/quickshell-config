import org.nightshell.Wayland
import org.nightshell.Services

import QtQuick

Item {
	id: root

	required property SessionLockManager lockManager
	property int currentTries: 0
	readonly property int maxTries: 3
	property string buffer
	property string message

	property bool locked

	signal unlockRequested

	Timer {
		id: lockoutTimeout
		interval: 10 * 60 * 1000
	}

	PamContext {
		id: pamContext

		onMessageChanged: {
			root.message = message;
		}

		onIsResponseRequiredChanged: {
			if (!isResponseRequired)
				return;

			respond(root.buffer);
			root.buffer = "";
		}

		onCompleted: result => {
			if (result == PamResult.Success) {
				return root.unlockRequested();
			} else {
				root.currentTries += 1;
			}
		}
	}

	Connections {
		target: root.lockManager
		function onIsSecureChanged() {
			if (root.lockManager.isSecure) {
				root.buffer = "";
				root.message = "";
			}
		}
	}
}
