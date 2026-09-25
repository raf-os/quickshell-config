import qs.services

import QtQuick
import QtQuick.Effects

MultiEffect {
	id: root

	readonly property bool shouldEnable: !GameMode.isActive
}
