import qs.components

import org.nightshell.Configs

import QtQuick

Item {
  id: root

  property Item target: null
  readonly property bool isActive: !!target

  implicitWidth: tooltipText.width
  implicitHeight: tooltipText.height

  transformOrigin: Item.Center
  transform: [
    Rotation {
      angle: 45
      axis: Qt.vector3d(0, 0, 1)
      origin: Qt.vector2d(root.implicitWidth / 2, root.implicitHeight / 2)
    }
  ]

  enum Anchor {
    Top,
    Bottom,
    Left,
    Right
  }

  onTargetChanged: {
    if (target === null || target === undefined)
      return;

    const tPos = target.mapToItem(root.parent, Qt.rect(0, 0, target.width, target.height));
  }

  StyledText {
    id: tooltipText
    text: "TEST"
    anchors.centerIn: parent
  }
}
