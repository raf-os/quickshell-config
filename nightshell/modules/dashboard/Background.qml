pragma ComponentBehavior: Bound

import org.nightshell.Utils
import org.nightshell.Configs

import QtQuick
import QtQuick.Shapes

Shape {
  id: root

  required property Content contentItem
  required property Item fillItem

  readonly property AbsoluteTransform titleTransform: contentItem.titleTransform
  readonly property AbsoluteTransform layoutTransform: contentItem.layoutTransform
  readonly property AbsoluteTransform sideTransform: contentItem.sideTransform
  readonly property int chamferSize: 10

  // qmlformat off
	readonly property rect titleRect: {
    return Qt.rect(
      titleTransform.x,
      titleTransform.y,
      Math.min(titleTransform.width, root.width),
      Math.min(titleTransform.height - 1, root.height)
    );
  }

	readonly property rect bodyRect: {
    return Qt.rect(
      layoutTransform.x,
      titleRect.y + titleRect.height,
      Math.min(layoutTransform.width, root.width),
      Math.min(layoutTransform.height, root.height)
    );
	}

  readonly property rect sideRect: {
    return Qt.rect(
      Math.min(sideTransform.x + 1, root.width),
      Math.min(sideTransform.y, root.height),
      Math.min(sideTransform.xEnd - 1, root.width),
      Math.min(sideTransform.yEnd, root.height)
    );
  }
  // qmlformat on

  readonly property real sideRectBevelLength: sideRect.width - sideRect.x

  // Main shape
  ShapePath {
    id: mainPath

    // fillColor: Colors.primaryContent
    fillItem: root.fillItem
    strokeColor: Colors.primary
    strokeWidth: 1
    joinStyle: ShapePath.MiterJoin

    // Top line
    PathLine {
      x: root.titleRect.x + root.titleRect.width
      y: root.titleRect.y
    }
    // Diagonal line
    PathLine {
      x: Math.min(root.width, root.titleRect.x + root.titleRect.width + root.titleRect.height)
      y: Math.min(root.height, root.titleRect.y + root.titleRect.height)
    }
    // Horizontal line, joining with body
    PathLine {
      x: root.bodyRect.x + root.bodyRect.width
      y: root.bodyRect.y
    }
    // Bevel, side buttons
    PathLine {
      x: root.sideRect.width
      y: root.sideRect.y + root.sideRectBevelLength
    }
    PathLine {
      relativeX: 0
      y: root.sideRect.height - root.sideRectBevelLength
    }
    PathLine {
      x: root.sideRect.x
      y: root.sideRect.height
    }
    // Vertical line, right side
    PathLine {
      x: root.sideRect.x
      y: root.bodyRect.y + Math.max(0, root.bodyRect.height - 24)
    }
    // Diagonal bevel, bottom right
    PathLine {
      x: Math.max(0, root.sideRect.x - 24)
      y: root.bodyRect.y + root.bodyRect.height
    }
    // Horizontal line, bottom
    PathLine {
      x: root.bodyRect.x
      relativeY: 0
    }
    // Vertical line, left, connecting to title
    PathLine {
      x: root.bodyRect.x
      y: root.titleRect.y
    }
  }
}
