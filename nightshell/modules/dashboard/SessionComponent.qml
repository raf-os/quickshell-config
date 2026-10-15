pragma ComponentBehavior: Bound

import qs.components

import org.nightshell.Configs
import org.nightshell.Services
import org.nightshell.IpcServer

import QtQuick
import QtQuick.Layouts

Item {
  id: root

  readonly property int padding: Styles.padding_md

  Layout.fillWidth: true

  implicitHeight: mainLayout.height + padding * 2

  signal closeRequested

  RowLayout {
    id: mainLayout

    anchors.centerIn: parent

    Btn {
      icon: "session_power"

      function onSelected() {
        SessionManager.shutdown();
      }
    }

    Btn {
      icon: "session_reboot"

      function onSelected() {
        SessionManager.reboot();
      }
    }

    Btn {
      icon: "session_lock"

      function onSelected() {
        IPCServer.SessionLock();
      }
    }

    Btn {
      icon: "settings"
    }

    Btn {
      icon: "launcher"
    }
  }

  component Btn: SMouseArea {
    id: cBtn
    required property string icon
    implicitWidth: 24
    implicitHeight: 24

    enabled: !SessionManager.isPending
    hoverEnabled: true

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
}
