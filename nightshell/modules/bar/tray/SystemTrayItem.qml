import org.nightshell.Configs
import org.nightshell.SystemTray
import org.nightshell.Components

import QtQuick
import QtQuick.Effects

MouseArea {
  id: root

  required property StatusNotifierItem modelData
  required property bool isActive
  readonly property bool isUrgent: modelData.status === StatusStatus.NeedsAttention
  property int padding: 4

  acceptedButtons: Qt.AllButtons
  hoverEnabled: true
  cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

  implicitWidth: implicitHeight

  signal openAttachedMenu
  signal requestClose

  SToolTip {
    active: !root.isActive && root.containsMouse
    text: root.modelData.tooltipTitle !== "" ? root.modelData.tooltipTitle : root.modelData.title
    offset: 4
  }

  onClicked: ev => {
    if (ev.button === Qt.LeftButton) {
      if (root.modelData.isMenuOnly) {
        root.openAttachedMenu();
      } else {
        root.modelData.activate();
        root.requestClose();
      }
    } else if (ev.button === Qt.MiddleButton) {
      if (root.modelData.isMenuOnly)
        return;
      root.modelData.secondaryActivate();
      root.requestClose();
    } else if (ev.button === Qt.RightButton) {
      root.openAttachedMenu();
    }
  }

  onModelDataChanged: {
    if (!modelData)
      root.requestClose();
  }

  Image {
    id: trayItemIcon
    visible: false
    anchors.centerIn: parent

    asynchronous: true
    width: parent.width - root.padding * 2
    height: parent.height - root.padding * 2

    source: root.modelData.iconUrl
  }

  MultiEffect {
    source: trayItemIcon
    anchors.fill: trayItemIcon
    colorization: 1
    colorizationColor: root.isUrgent ? Colors.accent : (root.isActive ? Colors.secondary : Colors.primary)
    contrast: 0.5
    brightness: 0.1
  }
}
