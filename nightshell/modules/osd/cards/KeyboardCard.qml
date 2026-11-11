import ".."

import org.nightshell.Hyprland

import QtQuick

CardBase {
  id: root

  readonly property HyprKeyboardLayout activeLayout: Hyprland.inputConfig.layouts[Hyprland.keyboardLayoutIndex] ?? Hyprland.inputConfig.layouts[0] ?? null

  CardIcon {
    icon: "keyboard"
  }

  CardContent {
    title: "Keyboard layout changed"
    content: root.activeLayout?.description ?? "Unknown keyboard layout"
  }
}
