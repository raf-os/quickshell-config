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

	LockAuth {
		id: lockAuth

		lockManager: lockManager
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

			onUnlockRequested: {
				if (unlockAnimTimer.running)
					return;

				lockManager.shouldBeActive = false;
				unlockAnimTimer.start();
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
