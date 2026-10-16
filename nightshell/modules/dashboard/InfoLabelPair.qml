import qs.components

import org.nightshell.Configs

import QtQuick
import QtQuick.Layouts

ColumnLayout {
  id: root

  required property string label
  required property string text

  spacing: Styles.spacing_xs

  property color label_color: Colors.neutralContent
  property color text_color: Colors.neutralContent

  property var horizontalAlignment: Text.AlignLeft

  StyledText {
    id: labelText

    Layout.fillWidth: true

    text: root.label
    font.pointSize: Styles.text_xs
    font.capitalization: Font.AllUppercase
    font.weight: 600

    color: root.label_color
    horizontalAlignment: root.horizontalAlignment
  }

  StyledText {
    id: textText

    Layout.fillWidth: true

    text: root.text
    font.pointSize: Styles.text_md
    font.weight: 600

    color: root.text_color
    horizontalAlignment: root.horizontalAlignment
  }
}
