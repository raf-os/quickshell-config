pragma ComponentBehavior: Bound

import org.nightshell.Utils

import qs.utils
import qs.components

import QtQuick

Item {
	id: root

	required property InstanceContext context
	property bool isActive: false

	FocusGrabber {
		active: root.isActive
		onFocusLost: root.isActive = false
	}

	Connections {
		target: root.context

		function onRequestToggleDashboard() {
			root.isActive = !root.isActive;
		}
	}

	Loader {
		id: contentLoader

		active: false

		readonly property bool shouldBeActive: root.isActive

		onShouldBeActiveChanged: {
			if (shouldBeActive)
				active = true;
		}

		sourceComponent: Content {
			id: content

			isActive: root.isActive

			onExitAnimationFinished: contentLoader.active = false
		}
	}
}
