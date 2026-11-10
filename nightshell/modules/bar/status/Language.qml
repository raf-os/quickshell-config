import "popouts"
import qs.components

import org.nightshell.Configs

import QtQuick
import QtQuick.Effects
import QtQuick.Layouts

BaseStatus {
  id: root
  name: "language"

  readonly property int hPadding: Styles.padding_xs
  readonly property int vPadding: Styles.padding_xs

  tooltipText: "Keyboard Layout"

  popoutComponent: LanguagePopout {}

  implicitWidth: langText.width
  Layout.fillHeight: true

  StyledText {
    id: langText

    anchors {
      top: parent.top
      bottom: parent.bottom
      horizontalCenter: parent.horizontalCenter
    }

    leftPadding: root.hPadding
    rightPadding: root.hPadding

    font.weight: 700
    font.pixelSize: parent.height * 0.75
    text: "us"

    color: root.isActive ? Colors.accent : Colors.primary
    Behavior on color {
      CAnim {
        duration: 400
      }
    }

    verticalAlignment: Text.AlignVCenter
  }

  SimpleGlowEffect {
    source: langText
    anchors.fill: langText
    colorizationColor: langText.color
    shadowColor: langText.color
  }
}
