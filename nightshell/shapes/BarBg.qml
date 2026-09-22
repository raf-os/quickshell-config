import qs.modules.bar

import org.nightshell.Configs as C
import QtQuick
import QtQuick.Shapes

ShapePath {
	id: root
	required property Bar bar
	readonly property int centerLength: bar.centerLength
	required property int width
	required property int height
	readonly property int slashThickness: 4

	strokeWidth: 0
	strokeColor: "transparent"
	fillColor: C.Colors.primary

	pathHints: ShapePath.PathFillOnRight | ShapePath.PathLinear

	PathMove {
		x: (root.width / 2) - (root.centerLength / 2)
		y: 0
	}
}
