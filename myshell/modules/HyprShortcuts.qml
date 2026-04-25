import qs.components.utils
import qs.services
import qs.utils
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
        //TODO: this
        name: "startmenuCommand"
        description: "Toggle start menu in command mode"

        onPressed: {
            root.startMenuInterrupted = true;
        }
        onReleased: {
            const openPanels = PanelService.getForActive();
            if (openPanels.startmenu === true)
                return;
            openPanels.startmenu = true;
            openPanels.desiredStartMenuTab = "command";
        }
    }

    CustomShortcut {
        name: "startMenuInterrupt"
        description: "Interrupt start menu keybind"
        onPressed: root.startMenuInterrupted = true
    }

    CustomShortcut {
        name: "toggleGamemode"
        description: "Toggles game mode on/off"
        onPressed: GlobalStateManager.toggleGamemodeState()
    }
}
