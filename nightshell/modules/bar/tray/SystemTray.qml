pragma ComponentBehavior: Bound

import qs.components

import org.nightshell.SystemTray
import org.nightshell.Configs
import org.nightshell.Components

import QtQuick
import QtQuick.Layouts

Item {
  id: root

  readonly property bool hasItems: SystemTrayQML.items.values.length > 0
  property SystemTrayItem current: null
  readonly property int padding: 4

  readonly property int vPadding: 1
  readonly property int hPadding: Styles.padding_sm

  implicitWidth: trayContents.implicitWidth + (hasItems ? hPadding + height / 6 : 0)
  Layout.fillHeight: true

  function toggleItem(item: SystemTrayItem): void {
    activationCooldown.running = true;
    if (root.current === item) {
      root.current = null;
    } else {
      root.current = item;
    }
  }

  function closePopup() {
    root.current = null;
  }

  Timer {
    id: activationCooldown
    interval: 500
  }

  ChamferRect {
    anchors.fill: root

    color: Colors.primaryContent
    borderWidth: 1
    borderColor: Colors.primary

    bottomRightChamfer: Math.round(height / 3)
  }

  ListView {
    id: trayContents

    acceptedButtons: Qt.NoButton
    orientation: ListView.Horizontal

    implicitWidth: contentWidth

    anchors {
      top: parent.top
      bottom: parent.bottom
      left: parent.left
      topMargin: root.vPadding
      bottomMargin: root.vPadding
      leftMargin: root.hPadding
    }

    model: SystemTrayQML.items
    delegate: SystemTrayItem {
      id: sysTrayItem
      isActive: root.current === this

      implicitHeight: ListView.view ? ListView.view.height : 0
      padding: root.padding

      onOpenAttachedMenu: {
        if (activationCooldown.running)
          return;
        root.toggleItem(sysTrayItem);
      }

      onRequestClose: {
        root.closePopup();
      }
    }
  }
}
