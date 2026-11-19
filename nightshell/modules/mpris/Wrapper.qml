pragma ComponentBehavior: Bound

import org.nightshell.Mpris
import org.nightshell.Configs

import QtQuick

Item {
  id: root

  readonly property bool isActive: Mpris.model.values.length > 0

  enum HPosition {
    Left = 0,
    Right = 1
  }

  enum VPosition {
    Top = 0,
    Bottom = 1
  }

  readonly property vector2d anchorPos: {
    switch (Config.modules.mpris.position) {
    case "TopLeft":
      return Qt.vector2d(Wrapper.Left, Wrapper.Top);
    case "BottomLeft":
      return Qt.vector2d(Wrapper.Left, Wrapper.Bottom);
    case "TopRight":
      return Qt.vector2d(Wrapper.Right, Wrapper.Top);
    case "BottomRight":
      return Qt.vector2d(Wrapper.Right, Wrapper.Bottom);
    default:
      return Qt.vector2d(Wrapper.Left, Wrapper.Top);
    }
  }

  anchors {
    left: root.anchorPos.x === Wrapper.Left ? parent.left : undefined
    right: root.anchorPos.x === Wrapper.Right ? parent.right : undefined
    top: root.anchorPos.y === Wrapper.Top ? parent.top : undefined
    bottom: root.anchorPos.y === Wrapper.Bottom ? parent.bottom : undefined
  }

  implicitWidth: loaderWrapper.width
  implicitHeight: loaderWrapper.height

  Loader {
    id: loaderWrapper

    property bool shouldBeActive: root.isActive

    onShouldBeActiveChanged: {
      if (shouldBeActive) {
        active = true;
      }
    }

    active: root.isActive
    sourceComponent: Content {
      isActive: root.isActive
      anchoring: root.anchorPos

      onExitAnimationFinished: {
        loaderWrapper.active = false;
      }
    }
  }
}
