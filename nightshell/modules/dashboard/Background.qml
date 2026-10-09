pragma ComponentBehavior: Bound

import org.nightshell.Utils
import org.nightshell.Configs

import QtQuick
import QtQuick.Shapes

Shape {
	id: root

	required property Content contentItem

	readonly property AbsoluteTransform titleTransform: contentItem.titleTransform
	readonly property AbsoluteTransform userInfoTransform: contentItem.userInfoTransform
	readonly property int chamferSize: 10

	P {
		startX: 0
		startY: 0
		maxWidth: root.width
		maxHeight: root.height
		fillColor: Colors.primary
	}

	P {
		padding: 1
		bodyWidth: maxWidth - 8
	}

	component P: ShapePath {
		id: bgPath

		fillColor: Colors.primaryContent
		strokeColor: "transparent"
		strokeWidth: 0

		property int maxWidth: root.width - padding * 2
		property int maxHeight: root.height - padding * 2
		property int bodyWidth: maxWidth

		property int padding: 0
		startX: padding
		startY: padding

		PathLine {
			relativeX: Math.min(root.titleTransform.xEnd - bgPath.padding, bgPath.maxWidth)
			relativeY: 0
		}
		PathLine {
			x: Math.min(bgPath.maxWidth, root.titleTransform.xEnd + root.titleTransform.height)
			y: Math.min(bgPath.maxHeight, root.titleTransform.yEnd + bgPath.padding)
		}
		PathLine {
			x: bgPath.startX
			relativeY: 0
		}
		PathLine {
			relativeX: 0
			y: bgPath.startY
		}

		PathMove {
			x: bgPath.startX
			y: bgPath.startY + Math.min(bgPath.maxHeight, root.userInfoTransform.y)
		}
		PathRectangle {
			relativeX: 0
			relativeY: 0
			width: Math.max(0, bgPath.bodyWidth)
			height: Math.max(0, root.userInfoTransform.height)
			bevel: true
			topRightRadius: Math.min(Math.min(width, height), 12)
			bottomRightRadius: 12
		}
	}
}
