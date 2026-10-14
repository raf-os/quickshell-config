pragma ComponentBehavior: Bound

import org.nightshell.Utils
import org.nightshell.Configs

import QtQuick
import QtQuick.Shapes

Shape {
  id: root

  required property Content contentItem
  required property Item fillItem

  readonly property AbsoluteTransform sessionTransform: contentItem.sessionTransform

  readonly property int leftWidth: 24
  readonly property int borderLeftBevel: Math.min(root.height * 0.5, 6)
  readonly property real bottomRightBevel: Math.min(root.height / 2, 12)

  readonly property bool shouldOffset: rendererType === Shape.CurveRenderer

  preferredRendererType: Shape.CurveRenderer

  // left thing
  ShapePath {
    id: borderLeft

    fillColor: Colors.primaryMuted
    strokeWidth: 1
    strokeColor: Colors.primary
    joinStyle: ShapePath.BevelJoin

    readonly property real topDepth: Math.min(Math.floor(root.height * 0.5), 3)
    readonly property int connectionHeight: 42
    readonly property int connectionBevelLength: Math.max(0, Math.min(4, root.height - (connectionHeight * 2)))

    PathMove {
      x: -root.leftWidth
      y: borderLeft.topDepth
    }
    PathLine {
      relativeX: root.leftWidth * 0.5
      relativeY: 0
    }
    PathLine {
      relativeX: borderLeft.topDepth
      y: 0
    }
    PathLine {
      x: -2
      y: 0
    }
    PathLine {
      relativeX: 0
      y: Math.min(borderLeft.connectionHeight, root.height)
    }
    PathLine {
      relativeX: -borderLeft.connectionBevelLength
      relativeY: borderLeft.connectionBevelLength
    }
    PathLine {
      relativeX: 0
      y: Math.max(0, root.height - borderLeft.connectionHeight - borderLeft.connectionBevelLength)
    }
    PathLine {
      relativeX: borderLeft.connectionBevelLength
      relativeY: borderLeft.connectionBevelLength
    }
    PathLine {
      relativeX: 0
      y: root.height
    }
    PathLine {
      x: -root.leftWidth + root.borderLeftBevel
      relativeY: 0
    }
    PathLine {
      relativeX: -root.borderLeftBevel
      relativeY: -root.borderLeftBevel
    }
    PathLine {
      x: -root.leftWidth
      y: borderLeft.topDepth
    }
  }

  // main content
  ShapePath {
    strokeColor: Colors.primary
    strokeWidth: 1
    fillColor: Colors.primaryContent
    joinStyle: ShapePath.BevelJoin

    PathRectangle {
      x: 0
      y: 0
      width: root.width
      height: root.height
      bevel: true
      bottomRightRadius: root.bottomRightBevel
    }
  }

  // session section
  ShapePath {
    id: ss

    readonly property real bevelLength: Math.min(root.width / 2, root.bottomRightBevel)

    startX: 0
    startY: root.sessionTransform.y

    fillColor: Colors.primary
    strokeWidth: 0
    strokeColor: "transparent"

    PathLine {
      x: Math.max(0, root.width - ss.bevelLength)
      relativeY: 0
    }
    PathLine {
      x: root.width
      y: ss.startY - ss.bevelLength
    }
    PathLine {
      relativeX: 0
      y: Math.min(root.height, root.height - ss.bevelLength)
    }
    PathLine {
      relativeX: -ss.bevelLength
      y: root.height
    }
    PathLine {
      x: 0
      y: root.height
    }
  }
}
