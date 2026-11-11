pragma ComponentBehavior: Bound

import qs.components
import qs.components.animations
import qs.modules as Modules

import org.nightshell.Configs
import org.nightshell.Utils
import org.nightshell.SystemTray

import QtQuick
import QtQuick.Controls

Item {
  id: root

  required property SystemTray systemTray
  required property Modules.Content content

  readonly property int padding: Styles.padding_sm
  readonly property int popoutWidth: 240
  readonly property int animDuration: 300
  readonly property int animFlickerDuration: 500

  readonly property int yOffset: Styles.spacing_sm

  property bool enableMovementAnim: false

  signal exitAnimationFinished

  function pushIndex(index: var, depth: int, statusItem: StatusNotifierItem) {
    popoutCurrent.push(submenuComponent, {
      statusItem: statusItem,
      depth: depth + 1,
      rootIndex: index
    });
  }

  Behavior on x {
    enabled: root.enableMovementAnim
    NAnim {
      duration: root.animDuration
    }
  }

  Behavior on y {
    enabled: root.enableMovementAnim
    NAnim {
      duration: root.animDuration
    }
  }

  Connections {
    target: root.systemTray
    function onCurrentChanged() {
      if (!root.systemTray.current) {
        root.enableMovementAnim = false;
        return;
      }

      const cur = root.systemTray.current;
      const newPos = cur.mapToItem(root.content, cur.width / 2, cur.height + root.yOffset);
      root.x = newPos.x;
      root.y = newPos.y;
      root.enableMovementAnim = true;
    }
  }

  FocusGrabber {
    id: focusGrab
    active: root.systemTray.current !== null

    onFocusLost: {
      root.systemTray.closePopup();
    }
  }

  MouseArea {
    id: popoutContent

    readonly property int padding: Styles.padding_md

    anchors {
      top: parent.bottom
      horizontalCenter: parent.horizontalCenter
    }

    implicitWidth: root.popoutWidth
    implicitHeight: popoutCurrent.currentItem ? popoutCurrent.currentItem.implicitHeight + padding * 2 : 0

    Behavior on implicitHeight {
      NAnim {
        duration: root.animDuration
      }
    }

    // opacity: root.systemTray.current ? 1 : 0

    Rectangle {
      anchors.fill: parent
      color: Colors.primaryContent
      border.width: 1
      border.color: Colors.primary
    }

    StackView {
      id: popoutCurrent

      anchors.fill: parent
      anchors.margins: popoutContent.padding
      clip: true

      popExit: ExitTransition {}

      replaceEnter: Transition {
        FlickerPopIn {
          duration: root.animFlickerDuration
        }
      }

      replaceExit: ExitTransition {}

      Connections {
        target: root.systemTray

        function onCurrentChanged() {
          if (root.systemTray.current) {
            popoutCurrent.replace(submenuComponent, {
              statusItem: root.systemTray.current.modelData,
              rootIndex: root.systemTray.current.modelData.menuHandle.rootIndex,
              depth: 0
            }, StackView.ReplaceTransition);
          } else {
            popoutCurrent.clear(StackView.PopTransition);
          }
        }
      }
    }
  }

  Component {
    id: submenuComponent
    Submenu {
      implicitWidth: StackView.view ? StackView.view.width : 0

      onNavigateToIndex: (index, statusItem) => {
        root.pushIndex(index, depth, statusItem);
      }

      onNavigateBack: {
        if (popoutCurrent.depth > 1) {
          popoutCurrent.popCurrentItem();
        } else {
          popoutCurrent.clear(StackView.PopTransition);
        }
      }

      onRequestClose: {
        root.systemTray.closePopup();
      }
    }
  }

  component ExitTransition: Transition {
    NumberAnimation {
      property: "opacity"
      from: 1
      to: 0
      duration: 200
    }
  }
}
