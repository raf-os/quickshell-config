pragma ComponentBehavior: Bound

import qs.components
import qs.utils

import org.nightshell.Configs
import org.nightshell.Services
import org.nightshell.IpcServer

import QtQuick
import QtQuick.Layouts

Item {
  id: root

  required property InstanceContext context

  readonly property int padding: Styles.padding_md
  // "Content"'s layerWrapper has its clip property set to true, so the visual parent must be Content itself
  required property Item tooltipVisualParent
  required property bool isActive

  Layout.fillWidth: true

  implicitHeight: mainLayout.height + padding * 2

  signal closeRequested

  RowLayout {
    id: mainLayout

    anchors.centerIn: parent

    Btn {
      icon: "session_power"
      tooltipContent: "Power Off"

      function onSelected() {
        SessionManager.shutdown();
      }
    }

    Btn {
      icon: "session_reboot"
      tooltipContent: "Reboot"

      function onSelected() {
        SessionManager.reboot();
      }
    }

    Btn {
      icon: "session_lock"
      tooltipContent: "Lock Session"

      function onSelected() {
        IPCServer.SessionLock();
      }
    }

    Btn {
      icon: "settings"
      tooltipContent: "Settings"
    }

    Btn {
      icon: "launcher"
      tooltipContent: "Launcher"

      function onSelected() {
        root.context.requestToggleLauncher();
      }
    }
  }

  component Btn: SMouseArea {
    id: cBtn
    required property string icon
    property string tooltipContent
    implicitWidth: 24
    implicitHeight: 24

    enabled: !SessionManager.isPending
    hoverEnabled: true

    TooltipTarget {
      id: tTarget
      tooltip: _tooltip
      content: cBtn.tooltipContent
    }

    function onSelected() {
    }

    onClicked: {
      root.closeRequested();
      onSelected();
    }

    ShellIcon {
      anchors.centerIn: parent
      size: 24
      icon: cBtn.icon

      colorize: true
      color: cBtn.containsMouse ? Colors.secondary : Colors.primaryContent
      scale: cBtn.containsMouse ? 1.1 : 1

      Behavior on scale {
        NAnim {
          duration: 200
        }
      }
      Behavior on color {
        CAnim {
          duration: 200
        }
      }
    }
  }

  Tooltip {
    id: _tooltip
    parent: root.tooltipVisualParent
    active: root.isActive
    offset: 2

    showDelay: 500
  }
}
