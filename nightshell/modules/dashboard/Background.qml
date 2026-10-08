import org.nightshell.Configs

import QtQuick
import QtQuick.Shapes

Shape {
	id: root

	required property Content contentItem

	ShapePath {
		id: panelPath

		fillColor: Colors.primaryContent
		strokeColor: Colors.primary
		strokeWidth: 1

		PathLine {
			relativeX: root.contentItem.titleWrapper.width
			relativeY: 0
		}
		PathLine {
			relativeX: 0
			relativeY: root.height
		}
		PathLine {
			relativeX: -root.width
			relativeY: 0
		}
		PathLine {
			relativeX: 0
			relativeY: -root.height
		}
	}
}
