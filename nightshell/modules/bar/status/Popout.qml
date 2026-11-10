import qs.components
import qs.utils

import org.nightshell.Configs
import org.nightshell.Utils

import QtQuick
import QtQuick.Controls

Item {
  id: root

  required property InstanceContext context
  required property Main statusWrapper
  readonly property BaseStatus current: statusWrapper.currentActive
  readonly property bool hasPopout: current ? current.popoutComponent : false
  property string currentName

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
      if (cur && root.hasPopout) {
        root.currentName = cur.name;
        const pos = cur.mapToItem(root.context.content, cur.width / 2, cur.height);
        root.x = pos.x;
        // root.y = pos.y;
        if (root.hasPopout) {
          root.openPopout(cur.popoutComponent);
        }
      } else {
        root.currentName = "";
        root.closePopout();
      }
    }
  }

  function openPopout(comp: Component): bool {
    if (!comp) {
      closePopout();
      return false;
    }

    const hadPrevious = stack.currentItem !== null && stack.currentItem !== undefined;

    stack.replaceCurrentItem(comp, {}, hadPrevious ? StackView.ReplaceTransition : StackView.PopTransition);
    return true;
  }

  function closePopout() {
    root.statusWrapper.requestCloseByName(root.currentName);
    stack.clear(StackView.PopTransition);
  }

  MouseArea {
    id: content

    property real bufferedWidth: 320
    enabled: root.hasPopout

    anchors {
      top: parent.bottom
      horizontalCenter: parent.horizontalCenter
    }

    implicitWidth: bufferedWidth
    implicitHeight: stack.currentItem ? Math.max(64, stack.currentItem.implicitHeight) : 0

    clip: true

    Behavior on implicitHeight {
      NAnim {
        duration: 600
      }
    }

    Connections {
      target: stack
      function onCurrentItemChanged() {
        const item = stack.currentItem;
        if (!item)
          return;

        if (item.implicitWidth === 0)
          content.bufferedWidth = 320;
        else
          content.bufferedWidth = item.implicitWidth;
      }
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
        NumberAnimation {
          property: "opacity"
          from: 0
          to: 1
          easing.type: Easing.InElastic
          easing.amplitude: 0.5
          easing.period: 0.4
          duration: 500
        }
      }

      replaceExit: Transition {
        NumberAnimation {
          property: "opacity"
          from: 1
          to: 0
          duration: 200
        }
      }

      popEnter: Transition {
        NumberAnimation {
          property: "opacity"
          from: 0
          to: 1
          easing.type: Easing.InElastic
          easing.amplitude: 0.5
          easing.period: 0.4
          duration: 500
        }
      }

      popExit: Transition {
        NAnim {
          property: "opacity"
          from: 1
          to: 0
          duration: 500
        }
      }
    }
  }
}
