import qs.components

import org.nightshell.Configs

import QtQuick
import QtQuick.Layouts

Item {
  id: root

  implicitWidth: 32
  implicitHeight: mainLayout.height + 32 * 2

  Rectangle {
    anchors.fill: parent
    color: Colors.primary
  }

  ColumnLayout {
    id: mainLayout

    anchors.centerIn: parent

    ShellIcon {
      icon: "session_power"
      size: 18
    }

    ShellIcon {
      icon: "session_lock"
      size: 18
    }
  }
}
