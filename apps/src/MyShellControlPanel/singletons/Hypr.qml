pragma Singleton

import MyShellPlugin
import MyShellPlugin.Utils
import QtQuick

Item {
    id: root

    readonly property list<HyprKeyboardLayout> inputLayouts: hyprExtras.inputConfig.layouts
    readonly property int currentLayoutIndex: hyprExtras.kbdLayoutIndex
    readonly property HyprKeyboardLayout currentLayout: inputLayouts[currentLayoutIndex]

    readonly property list<KKeyboardLayout> allLayouts: keyboardLayoutHandler.layouts

    HyprExtras {
        id: hyprExtras
        keyboardLayoutHandler: keyboardLayoutHandler
        configPath: `${Paths.home}/.config/hypr`

        Component.onCompleted: {
            initConfigParse();
            updateCurrentKeyboardConfig();
        }
    }

    KeyboardLayoutHandler {
        id: keyboardLayoutHandler
    }
}
