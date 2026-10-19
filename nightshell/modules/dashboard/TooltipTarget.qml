import QtQuick

Item {
  id: root

  required property Tooltip tooltip
  property string content

  anchors.fill: parent

  function evalParent() {
    if (!(root.parent as MouseArea)) {
      console.warn("TooltipTarget MUST be a child of a MouseArea. Destroying object.");
      root.destroy();
    }
  }

  Component.onCompleted: {
    evalParent();
  }

  onParentChanged: {
    evalParent();
  }

  Connections {
    target: root.parent

    function onEntered() {
      root.tooltip.targetEnter(root);
    }

    function onExited() {
      root.tooltip.targetLeave(root);
    }
  }
}
