import QtQuick
import QtQuick.Controls

Item {
  anchors.fill: parent

  Sidebar {
    id: sidebar

    anchors.left: parent.left
    anchors.top: parent.top
    anchors.bottom: parent.bottom
  }

  StackView {
    id: mainPage

    anchors.left: sidebar.right
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.right: parent.right
  }
}
