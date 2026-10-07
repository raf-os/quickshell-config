pragma ComponentBehavior: Bound

import qs.components

import org.nightshell.Wayland
import org.nightshell.IpcServer

import QtQuick

Item {
	id: root

	Timer {
		id: unlockAnimTimer
		interval: 400
		onTriggered: {
			lockManager.unlock();
		}
	}

	function onUnlockRequested() {
		if (unlockAnimTimer.running)
			return;

		lockManager.shouldBeActive = false;
		unlockAnimTimer.start();
	}

	LockAuth {
		id: lockAuth

		lockManager: lockManager

		onUnlockRequested: root.onUnlockRequested()
	}

	SessionLockManager {
		id: lockManager

		property bool shouldBeActive: false
		onIsLockedChanged: {
			if (isLocked)
				shouldBeActive = true;
		}

		surface: SurfaceContent {
			isActive: lockManager.shouldBeActive
			lockAuth: lockAuth

			onUnlockRequested: root.onUnlockRequested()

			onPwTextChanged: {
				lockAuth.buffer = inputTextContent;
			}
		}
	}

	Connections {
		target: IPCServer
		function onSessionLockRequested() {
			lockManager.lock();
		}
	}
}
