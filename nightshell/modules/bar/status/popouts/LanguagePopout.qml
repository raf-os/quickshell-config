pragma ComponentBehavior: Bound

import qs.components

import org.nightshell.Hyprland
import org.nightshell.Configs

import QtQuick
import QtQuick.Effects
import QtQuick.Layouts

BasePopout {
  id: root

  // readonly property int hPadding: Styles.padding_lg
  // readonly property int vPadding: Styles.padding_lg
  readonly property bool isEnabled: !Hyprland.xkbSwitchCooldown

  implicitWidth: 240
  implicitHeight: rootLayout.height

  ColumnLayout {
    id: rootLayout

    spacing: Styles.spacing_md

    anchors {
      left: parent.left
      right: parent.right
      top: parent.top
      // leftMargin: root.hPadding
      // rightMargin: root.hPadding
      // topMargin: root.vPadding
    }

    StyledText {
      color: Colors.primary
      text: "Keyboard layout"

      font.pointSize: Styles.text_md
      font.weight: 600
    }

    SListView {
      id: langView
      Layout.fillWidth: true
      Layout.fillHeight: true

      currentIndex: Hyprland.keyboardLayoutIndex

      implicitHeight: Math.min(view.contentHeight, 320)

      model: Hyprland.inputConfig.layouts
      delegate: LayoutItem {}

      highlight: Item {
        id: lvh
        opacity: langView.currentItem ? 1 : 0

        RectangularShadow {
          anchors.fill: parent
          color: Colors.secondary
          opacity: root.isEnabled ? 1 : 0
        }
        Rectangle {
          anchors.fill: parent
          color: Colors.secondary
          opacity: root.isEnabled ? 1 : 0.25
          Behavior on opacity {
            NAnim {
              duration: 300
            }
          }
        }
      }

      Rectangle {
        anchors.fill: parent
        color: Qt.alpha(Colors.primary, 0.25)
        z: -10
      }

      RectangularShadow {
        z: -5
        visible: !!langView.currentItem
        color: Colors.secondary
      }
    }
  }

  component LayoutItem: SMouseArea {
    id: layoutItemComp
    required property HyprKeyboardLayout modelData
    required property int index

    readonly property bool isSelected: ListView.isCurrentItem
    enabled: root.isEnabled

    implicitWidth: ListView.view ? ListView.view.width : 0
    implicitHeight: layoutName.height

    onClicked: {
      Hyprland.switchKeyboardLayout(index);
    }

    StyledText {
      id: layoutName
      text: layoutItemComp.modelData.description

      leftPadding: Styles.padding_lg
      rightPadding: leftPadding
      topPadding: Styles.padding_md
      bottomPadding: topPadding

      font.pointSize: Styles.text_sm
      font.weight: layoutItemComp.isSelected ? 600 : 400
      color: layoutItemComp.isSelected ? Colors.secondaryContent : Colors.primary

      anchors {
        left: parent.left
        right: parent.right
        verticalCenter: parent.verticalCenter
      }
    }
  }
}
