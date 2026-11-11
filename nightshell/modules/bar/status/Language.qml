import "popouts"
import qs.components

import org.nightshell.Configs
import org.nightshell.Hyprland

import QtQuick
import QtQuick.Layouts

BaseStatus {
  id: root
  name: "language"

  readonly property int hPadding: Styles.padding_xs
  readonly property int vPadding: Styles.padding_xs

  tooltipText: "Keyboard Layout"

  popoutComponent: LanguagePopout {}

  implicitWidth: fontMetrics.averageCharacterWidth * 2 + hPadding * 2
  Layout.fillHeight: true

  FontMetrics {
    id: fontMetrics
    font: langText.font
  }

  StyledText {
    id: langText

    anchors.fill: parent

    // leftPadding: root.hPadding
    // rightPadding: root.hPadding

    font.weight: 700
    font.pixelSize: parent.height * 0.75
    text: {
      const layout = Hyprland.inputConfig.layouts[Hyprland.keyboardLayoutIndex];
      if (!layout)
        return "n/a";
      if (layout.variant !== "")
        return layout.variant;
      return layout.layout;
    }

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
