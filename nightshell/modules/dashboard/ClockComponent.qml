import qs.components

import org.nightshell.Configs
import org.nightshell.Services

import QtQuick
import QtQuick.Layouts

Item {
  id: root

  implicitWidth: mainLayout.implicitWidth
  implicitHeight: mainLayout.implicitHeight

  ColumnLayout {
    id: mainLayout

    width: root.width

    StyledText {
      Layout.alignment: Qt.AlignRight
      text: Qt.formatTime(SystemTime.date, "hh:mm")

      font.pointSize: Styles.text_xl * 1.25
      font.weight: 600

      color: Colors.primary
    }

    StyledText {
      Layout.alignment: Qt.AlignRight
      text: Qt.formatDate(SystemTime.date, Locale.LongFormat)

      font.pointSize: Styles.text_sm

      color: Colors.primary
    }
  }
}
