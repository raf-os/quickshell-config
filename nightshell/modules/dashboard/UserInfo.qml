import qs.components

import org.nightshell.Services
import org.nightshell.Utils
import org.nightshell.Configs

import QtQuick
import QtQuick.Layouts

Item {
  id: root

  implicitHeight: mainLayout.height

  RowLayout {
    id: mainLayout

    anchors {
      left: parent.left
      right: parent.right
      top: parent.top
    }

    spacing: Styles.spacing_lg

    Item {
      id: userImageWrapper

      implicitWidth: 96
      implicitHeight: 96

      Layout.fillHeight: true
      Layout.alignment: Qt.AlignVCenter

      Rectangle {
        anchors.fill: parent
        color: Colors.primaryMuted
      }
    }

    Item {
      id: userInfoContent

      Layout.fillWidth: true
      Layout.fillHeight: true

      implicitHeight: userInfoContentLayout.implicitHeight

      ColumnLayout {
        id: userInfoContentLayout

        implicitWidth: parent.implicitWidth
        spacing: Styles.spacing_md

        SInfoLabelPair {
          label: "NAME"
          text: UserData.name
        }

        SInfoLabelPair {
          label: "Operating System"
          text: SysInfo.name
        }

        SInfoLabelPair {
          label: "Uptime"
          text: "1 century"
        }
      }
    }
  }

  Tooltip {}

  component SInfoLabelPair: InfoLabelPair {
    label_color: Colors.primary
    text_color: Colors.primary
  }
}
