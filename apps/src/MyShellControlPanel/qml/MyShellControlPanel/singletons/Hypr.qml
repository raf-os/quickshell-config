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
    readonly property bool isKbSwitchOnCooldown: kbLayoutCooldown.running

    function switchKeyboardLayout(idx: int): void {
        if (kbLayoutCooldown.running)
            return;
        hyprExtras.hyprctl(["switchxkblayout", "current", String(idx)]);
        kbLayoutCooldown.restart();
    }

    Timer {
        id: kbLayoutCooldown
        interval: 500
    }

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
