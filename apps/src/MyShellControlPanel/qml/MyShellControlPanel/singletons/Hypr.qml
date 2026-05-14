pragma Singleton

import MyShellControlPanel.singletons
import MyShellPlugin as MP
import MyShellPlugin.Utils as MPUT
import QtQuick

Item {
    id: root

    readonly property list<MP.HyprKeyboardLayout> inputLayouts: hyprExtras.inputConfig.layouts
    readonly property int currentLayoutIndex: hyprExtras.kbdLayoutIndex
    readonly property MP.HyprKeyboardLayout currentLayout: inputLayouts[currentLayoutIndex] ?? null

    readonly property list<MP.KKeyboardLayout> allLayouts: KbdLayoutHandler.handler.layouts

    MP.HyprExtras {
        id: hyprExtras
        keyboardLayoutHandler: KbdLayoutHandler.handler
        configPath: `${MPUT.Paths.home}/.config/hypr`

        Component.onCompleted: {
            initConfigParse();
            updateCurrentKeyboardConfig();
        }
    }
}
