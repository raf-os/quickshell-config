pragma ComponentBehavior: Bound

import qs.components
import qs.services as Services
import qs.utils

import QtQuick

Item {
	id: root

	required property InstanceContext context

	property bool isActive: false
	property int currentType: 0

	enum Type {
		NONE = 0,
		Audio = 1,
		GameMode = 2,
		KeyboardLayout = 3,
		MAX = 4
	}

	Timer {
		id: osdTimeout
		interval: 5000
		onTriggered: {
			root.isActive = false;
		}
	}

	function trigger(type: int) {
		if (type <= 0 || type >= OSD.Type.MAX)
			return;

		root.isActive = true;
		root.currentType = type;
		osdTimeout.restart();
	}

	Connections {
		target: Services.GameMode
		function onIsActiveChanged() {
			root.trigger(OSD.Type.GameMode);
		}
	}

	Loader {
		id: osdSlot

		readonly property bool shouldBeActive: root.isActive
		active: false

		anchors {
			right: parent.right
			bottom: parent.bottom
		}

		onShouldBeActiveChanged: {
			if (shouldBeActive) {
				osdSlot.active = true;
			}
		}

		sourceComponent: OSDContent {
			isActive: root.isActive
			type: root.currentType
			context: root.context

			onExitAnimationFinished: {
				osdSlot.active = false;
			}
		}
	}
}
