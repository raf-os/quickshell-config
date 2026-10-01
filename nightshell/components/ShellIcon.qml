pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Effects

Image {
	id: root

	required property string icon
	required property int size
	property bool colorize: false
	property bool glow: false
	property color color

	asynchronous: true
	width: size
	height: size
	source: `image://qicons/shell/${icon}`
	sourceSize.width: size
	sourceSize.height: size

	layer.enabled: root.colorize
	layer.effect: SimpleGlowEffect {
		colorization: root.colorize ? 1 : 0
		colorizationColor: root.color
		shadowEnabled: root.glow && shouldEnable
		shadowOpacity: root.glow ? 1 : 0
		shadowColor: root.color
	}
}
