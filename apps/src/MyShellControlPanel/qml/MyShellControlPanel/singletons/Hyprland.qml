pragma Singleton

import MyShellPlugin as MP
import MyShellPlugin.Utils
import QtQuick

Item {
    id: root

    readonly property list<MP.HyprKeyboardLayout> inputLayouts: hyprExtras.inputConfig.layouts
    readonly property int currentLayoutIndex: hyprExtras.kbdLayoutIndex
    readonly property MP.HyprKeyboardLayout currentLayout: inputLayouts[currentLayoutIndex]

    readonly property list<MP.KKeyboardLayout> allLayouts: keyboardLayoutHandler.layouts

    MP.KeyboardLayoutHandler {
        id: keyboardLayoutHandler

        cachePath: Paths.cache
    }

    MP.HyprExtras {
        id: hyprExtras
        keyboardLayoutHandler: keyboardLayoutHandler
        configPath: `${Paths.home}/.config/hypr`

        Component.onCompleted: {
            initConfigParse();
            updateCurrentKeyboardConfig();
        }
    }
}
