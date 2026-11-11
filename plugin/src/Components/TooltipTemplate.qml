import org.nightshell.Configs

import QtQuick

Item {
  id: root

  property string text

  implicitWidth: Math.max(1, tooltipText.width)
  implicitHeight: Math.max(1, tooltipText.height)

  Rectangle {
    anchors.fill: parent

    color: Colors.primaryContent
    border.width: 1
    border.color: Colors.primary
  }

  Text {
    id: tooltipText

    anchors.centerIn: parent

    text: root.text
    font.family: Config.appearance.fontFamily.sans
    font.pointSize: Styles.text_sm
    color: Colors.primary

    padding: 4
  }
}
