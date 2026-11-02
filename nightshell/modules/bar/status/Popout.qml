import qs.components

import org.nightshell.Configs
import org.nightshell.Utils

import QtQuick
import QtQuick.Controls

Item {
  id: root

  required property Main statusWrapper
  readonly property BaseStatus current: statusWrapper.currentActive
  readonly property bool hasPopout: current ? current.popoutComponent : false

  signal closed

  FocusGrabber {
    id: focusGrab
    active: root.hasPopout

    onFocusLost: {
      root.closePopout();
    }
  }

  Connections {
    target: root.statusWrapper
    function onCurrentActiveChanged() {
      const cur = root.current;
      if (cur) {
        const pos = cur.mapToItem(root.statusWrapper, cur.width / 2, cur.height);
        root.x = pos.x;
        root.y = pos.y;
        if (root.hasPopout) {
          root.openPopout(cur.popoutComponent);
        }
      } else {
        root.closePopout();
      }
    }
  }

  function openPopout(comp: Component): bool {
    if (!comp) {
      closePopout();
      return false;
    }

    stack.replaceCurrentItem(comp, {}, StackView.ReplaceTransition);
    return true;
  }

  function closePopout() {
    root.closed();
    stack.clear(StackView.PopTransition);
  }

  Item {
    id: content

    anchors {
      top: parent.bottom
      horizontalCenter: parent.horizontalCenter
    }

    implicitWidth: 320
    implicitHeight: stack.currentItem ? stack.currentItem.implicitHeight : 0

    clip: true

    Behavior on implicitHeight {
      NAnim {}
    }

    Rectangle {
      anchors.fill: stack
      color: Colors.primaryContent
      border.width: 1
      border.color: Colors.primary
    }

    StackView {
      id: stack

      anchors.fill: parent

      replaceEnter: Transition {
        NAnim {
          property: "x"
          from: -32
          to: 0
          duration: 400
        }
      }

      replaceExit: Transition {
        NAnim {
          property: "x"
          from: 0
          to: 32
          duration: 400
        }
      }

      popExit: Transition {
        NAnim {
          property: "opacity"
          from: 1
          to: 0
          duration: 400
        }
      }
    }
  }
}
