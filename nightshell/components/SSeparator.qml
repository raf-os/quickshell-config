import org.nightshell.Configs

import QtQuick
import QtQuick.Layouts

Rectangle {
  id: root

  property int orientation: SSeparator.Orientation.Horizontal
  property int padding: Styles.padding_md
  color: Colors.neutralContent

  enum Orientation {
    Horizontal,
    Vertical
  }

  readonly property bool isHorizontal: orientation === SSeparator.Orientation.Horizontal
  readonly property bool isVertical: orientation === SSeparator.Orientation.Vertical

  Layout.fillWidth: isHorizontal ? true : false
  Layout.fillHeight: isVertical ? true : false

  Layout.leftMargin: isHorizontal ? padding : 0
  Layout.rightMargin: isHorizontal ? padding : 0

  Layout.topMargin: isVertical ? padding : 0
  Layout.bottomMargin: isVertical ? padding : 0

  implicitWidth: 1
  implicitHeight: 1
}
