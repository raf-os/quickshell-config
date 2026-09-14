pragma Singleton

import Quickshell
import QtQuick

Singleton {
	id: root

	PersistentProperties {
		id: props
		property bool isGamemodeActive: false
	}

	readonly property bool isGamemode: props.isGamemodeActive

	function toggleGamemode() {
		props.isGamemodeActive = !props.isGamemodeActive;
	}
}
