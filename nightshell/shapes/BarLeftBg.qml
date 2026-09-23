import qs.modules.bar
import qs.modules.bar.left as L

import org.nightshell.Configs
import QtQuick
import QtQuick.Shapes

ShapePath {
	id: root

	required property Bar bar
	readonly property L.Main leftComponent: bar.leftComponent
	readonly property Item osIcon: leftComponent.osIcon
	readonly property int itemHeight: bar.height - bar.padding * 2

	readonly property int osIconStartX: osIcon.x
	readonly property int osIconLength: osIcon.width

	readonly property int titleStartX: leftComponent.titleX
	readonly property int titleLength: leftComponent.titleWidth

	startX: root.bar.paddingH
	startY: bar.padding

	strokeWidth: 0
	strokeColor: "transparent"
	fillColor: Colors.secondary

	PathMove {
		x: root.osIconStartX + root.startX
		y: root.startY
	}
	PathRectangle {
		relativeX: 0
		relativeY: 0
		width: root.osIconLength
		height: root.itemHeight
		bevel: true
		bottomLeftRadius: 8
	}

	PathMove {
		x: root.titleStartX + root.startX
		y: root.startY
	}
	PathRectangle {
		relativeX: 0
		relativeY: 0
		width: root.titleLength
		height: root.itemHeight
		bevel: true
		bottomRightRadius: 8
	}
}
