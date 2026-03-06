import qs.components.utils
import qs.services
import QtQuick
import Quickshell

// Makes use of hyprland's "global shortcuts" so you can reroute hotkeys
// to this shell through quickshell's IPC

Scope {
    id: root

    property bool startMenuInterrupted
    readonly property bool hasFullscreen: Hypr.focusedWorkspace?.toplevels.values.some(t => t.lastIpcObject.fullscreen === 2) ?? false

    CustomShortcut {
        name: "startmenu"
        description: "Toggle start menu"

        onPressed: root.startMenuInterrupted = false
        onReleased: {
            if (!root.startMenuInterrupted && !root.hasFullscreen) {
                const openPanels = PanelService.getForActive();
                openPanels.startmenu = !openPanels.startmenu;
            }
            root.startMenuInterrupted = false;
        }
    }

    CustomShortcut {
        name: "startMenuInterrupt"
        description: "Interrupt start menu keybind"
        onPressed: root.startMenuInterrupted = true
    }
}
