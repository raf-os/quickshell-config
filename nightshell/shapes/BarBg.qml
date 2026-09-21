import qs.modules.bar

import org.nightshell.Configs as C
import QtQuick
import QtQuick.Shapes

ShapePath {
	id: root
	required property Bar bar
	required property int maxWidth
	required property int maxHeight
	required property Item centerItem
	readonly property int baseHeight: bar.topLineHeight
	readonly property int slashThickness: 4
	readonly property int centerLength: Math.max(10, centerItem.width)
	readonly property int centerHeight: centerItem.height - baseHeight

	strokeWidth: 0
	strokeColor: "transparent"
	fillColor: C.Colors.primary

	pathHints: ShapePath.PathFillOnRight | ShapePath.PathLinear

	// Line at top
	PathRectangle {
		x: 0
		y: 0
		width: root.maxWidth
		height: root.baseHeight
	}

	// Center component
	PathMove {
		x: (root.maxWidth / 2) - (root.centerLength / 2) - root.centerHeight
		y: root.baseHeight
	}
	PathLine {
		relativeX: root.centerLength + root.centerHeight * 2
		relativeY: 0
	}
	PathLine {
		relativeX: -root.centerHeight
		relativeY: root.centerHeight
	}
	PathLine {
		relativeX: -root.centerLength
		relativeY: 0
	}
	PathLine {
		relativeX: -root.centerHeight
		relativeY: -root.centerHeight
	}

	// Left slash
	PathMove {
		x: (root.maxWidth / 2) - (root.centerLength / 2) - root.centerHeight - root.slashThickness - 2
		y: root.baseHeight + 2
	}
	PathLine {
		relativeX: root.slashThickness
		relativeY: 0
	}
	PathLine {
		relativeX: root.centerHeight - 2
		relativeY: root.centerHeight - 2
	}
	PathLine {
		relativeX: -root.slashThickness
		relativeY: 0
	}

	// Right slash
	PathMove {
		x: (root.maxWidth / 2) + (root.centerLength / 2) + root.centerHeight + 2
		y: root.baseHeight + 2
	}
	PathLine {
		relativeX: root.slashThickness
		relativeY: 0
	}
	PathLine {
		relativeX: -root.centerHeight + 2
		relativeY: root.centerHeight - 2
	}
	PathLine {
		relativeX: -root.slashThickness
		relativeY: 0
	}
}
