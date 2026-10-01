pragma Singleton

import Quickshell
import QtQuick

Singleton {
	id: root

	property alias keyboardSwitchOnCooldown: keyboardLayoutSwitchCooldown.running

	function keyboardSwitchRequested(): bool {
		if (keyboardLayoutSwitchCooldown.running)
			return false;

		keyboardLayoutSwitchCooldown.start();
		return true;
	}

	Timer {
		id: keyboardLayoutSwitchCooldown
		interval: 2000
	}
}
