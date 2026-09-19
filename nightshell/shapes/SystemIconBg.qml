import qs.modules.bar

import QtQuick
import QtQuick.Shapes

ShapePath {
	id: root

	required property Bar bar
	readonly property int itemLength: 400
	readonly property int contentLength: 200
	readonly property int trailingLength: itemLength - contentLength
	readonly property int itemHeight: bar.height - bar.topLineHeight - 4
	readonly property int tailHeight: 8

	startX: 32
	startY: bar.topLineHeight + 4

	strokeWidth: 0
	strokeColor: "transparent"
	fillColor: "#1ee8e4"

	PathLine {
		relativeX: root.itemLength - root.tailHeight
		relativeY: 0
	}
	PathLine {
		relativeX: root.tailHeight
		relativeY: root.tailHeight
	}
	PathLine {
		relativeX: -root.trailingLength
		relativeY: 0
	}
	PathLine {
		relativeX: -(root.itemHeight - root.tailHeight)
		relativeY: (root.itemHeight - root.tailHeight)
	}
	PathLine {
		relativeX: -root.itemLength + root.trailingLength + (root.itemHeight - root.tailHeight)
		relativeY: 0
	}
}
