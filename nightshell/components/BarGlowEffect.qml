import qs.services

import QtQuick
import QtQuick.Effects

MultiEffect {
	id: root

	property bool shouldEnable: true
	enabled: !GameMode.isActive && shouldEnable

	blurEnabled: true
	blur: 1
	blurMax: 12
	blurMultiplier: 0.4
	brightness: 0.2
	contrast: 0.5
}
