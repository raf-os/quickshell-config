import qs.components

import org.nightshell.Configs

import QtQuick

Item {
  id: root

  enum Anchor {
    Top,
    Bottom,
    Left,
    Right
  }

  // What side of the target item should this tooltip be anchored to
  // (defaults to bottom)
  property int anchor: Tooltip.Anchor.Bottom
  // The distance from the target's anchor
  property int offset: 0

  // The active target - avoid setting manually
  property TooltipTarget target: null
  property string contentStr

  property bool active: true
  readonly property bool isActive: !!target
  property bool hasEntered: false

  // Amount of time mouse must be hovered over for the tooltip to show
  property int showDelay: 1000
  // Amount of time before tooltip is dismissed after mouse leaves
  property int exitGraceWindow: 250

  readonly property int animDuration: 300

  SequentialAnimation {
    id: enterAnimation

    ParallelAnimation {
      NumberAnimation {
        target: root
        property: "opacity"
        to: 1
        duration: root.animDuration
      }
    }
  }

  SequentialAnimation {
    id: exitAnimation

    ParallelAnimation {
      NumberAnimation {
        target: root
        property: "opacity"
        to: 0
        duration: root.animDuration
      }
    }
    ScriptAction {
      script: root.hasEntered = false
    }
  }

  opacity: 0

  onActiveChanged: {
    if (!active) {
      root.target = null;
      queuedEnter.stop();
      queuedLeave.stop();
      enterAnimation.stop();
      exitAnimation.restart();
    }
  }

  function getRotation(anchor: int): int {
    switch (anchor) {
    case Tooltip.Anchor.Left:
      return 90;
    case Tooltip.Anchor.Right:
      return 270;
    default:
      return 0;
    }
  }

  function targetEnter(target: TooltipTarget): void {
    if (!root.active)
      return;

    queuedLeave.stop();
    exitAnimation.stop();
    root.target = target;
    root.contentStr = target.content;

    if (!hasEntered) {
      queuedEnter.restart();
    } else {
      enterAnimation.start();
    }
  }
  function targetLeave(target: TooltipTarget): void {
    if (!root.active)
      return;

    if (root.target !== target)
      return;

    root.target = null;

    if (queuedEnter.running) {
      queuedEnter.stop();
    }
    queuedLeave.restart();
  }

  // fancy way of rotating + offsetting item relative to its new rotated coordinates
  transform: Matrix4x4 {
    matrix: {
      var t = Qt.matrix4x4(); // identity
      const rotation = root.getRotation(root.anchor);
      t.rotate(rotation, Qt.vector3d(0, 0, 1));
      t.translate(Qt.vector3d(0, root.offset * (root.anchor === Tooltip.Anchor.Top ? -1 : 1), 0));
      return t;
    }
  }

  onTargetChanged: {
    if (target === null || target === undefined)
      return;

    const tPos = target.mapToItem(root.parent, 0, 0);
    x = tPos.x + (root.anchor === Tooltip.Anchor.Left ? 0 : root.anchor === Tooltip.Anchor.Right ? target.width : target.width / 2);
    y = tPos.y + (root.anchor === Tooltip.Anchor.Top ? 0 : root.anchor === Tooltip.Anchor.Bottom ? target.height : target.height / 2);
  }

  Timer {
    id: queuedEnter
    interval: root.showDelay

    onTriggered: {
      root.hasEntered = true;
      exitAnimation.stop();
      enterAnimation.start();
    }
  }

  Timer {
    id: queuedLeave
    interval: root.exitGraceWindow

    onTriggered: {
      enterAnimation.stop();
      exitAnimation.start();
    }
  }

  Item {
    id: content

    readonly property bool isTopAnchor: root.anchor === Tooltip.Anchor.Top
    readonly property bool isBottomAnchor: root.anchor === Tooltip.Anchor.Bottom

    property real offset: root.hasEntered ? 0 : 8

    Behavior on offset {
      NAnim {
        duration: 300
      }
    }

    anchors {
      top: isTopAnchor ? undefined : parent.bottom
      topMargin: isBottomAnchor ? offset : 0
      bottom: isTopAnchor ? parent.top : undefined
      bottomMargin: isTopAnchor ? offset : 0
      horizontalCenter: parent.horizontalCenter
    }

    implicitWidth: tooltipText.width
    implicitHeight: tooltipText.height

    Rectangle {
      anchors.fill: parent
      color: Colors.primaryContent
      border.width: 1
      border.color: Colors.primaryMuted
    }

    StyledText {
      id: tooltipText
      text: root.contentStr

      font.pointSize: Styles.text_sm
      font.capitalization: Font.AllUppercase
      font.weight: 600

      padding: Styles.padding_md

      color: Colors.primary
    }
  }
}
