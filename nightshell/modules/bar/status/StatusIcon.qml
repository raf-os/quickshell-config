import qs.components

import QtQuick

Item {
  id: root
  required property string icon
  readonly property int size: Math.min(width, height)
  anchors.fill: parent

  Image {
    anchors.centerIn: parent
    width: root.size
    height: root.size
    // sourceSize.width: root.size
    // sourceSize.height: root.size
    source: `image://qicons/shell/${root.icon}`
  }
}
