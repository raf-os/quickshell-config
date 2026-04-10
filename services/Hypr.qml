pragma Singleton

import qs.utils
import MyShellPlugin
import Quickshell
import Quickshell.Hyprland
import QtQuick

Singleton {
    id: root

    readonly property var toplevels: Hyprland.toplevels
    readonly property var workspaces: Hyprland.workspaces
    readonly property var monitors: Hyprland.monitors

    readonly property HyprlandToplevel activeTopLevel: Hyprland.activeToplevel?.wayland?.activated ? Hyprland.activeToplevel : null
    readonly property HyprlandWorkspace focusedWorkspace: Hyprland.focusedWorkspace
    readonly property HyprlandMonitor focusedMonitor: Hyprland.focusedMonitor

    readonly property bool isKeyboardSwitchOnCooldown: keyboardSwitchCooldown.running

    property string lastKeyboardChangeInfo

    property int currentIndex: hyprExtras.kbdLayoutIndex ?? 0
    property string currentLayout: hyprExtras.inputConfig?.layouts[currentIndex]?.layout ?? "-"

    property var inputConfig: hyprExtras.inputConfig

    signal keyboardLayoutChanged

    function evalNewKeyboardChange(layoutStr: string): void {
        hyprExtras.updateCurrentKeyboardConfig();
        root.lastKeyboardChangeInfo = layoutStr;
        keyboardLayoutChanged();
    }

    function monitorFor(screen: ShellScreen): HyprlandMonitor {
        return Hyprland.monitorFor(screen);
    }

    function workspacesForScreen(screen: ShellScreen): list<int> {
        var monitor = Hyprland.monitorFor(screen);
        var ws = Hyprland.workspaces.values.reduce((workspaces, current) => {
            if (current?.monitor?.id === monitor.id)
                workspaces.push(current.id);
            return workspaces;
        }, []);

        return ws;
    }

    function switchKeyboardLayout(to: int) {
        if (keyboardSwitchCooldown.running)
            return;

        if (to === undefined || to === null) {
            Quickshell.execDetached(["hyprctl", "switchxkblayout", "current", "next"]);
        } else {
            Quickshell.execDetached(["hyprctl", "switchxkblayout", "current", String(to)]);
        }
        keyboardSwitchCooldown.restart();
    }

    Timer {
        id: keyboardSwitchCooldown
        interval: 500
    }

    Connections {
        target: Hyprland

        function onRawEvent(event: HyprlandEvent): void {
            const n = event.name;
            if (n.endsWith("v2"))
                return;

            if (n.includes("activelayout")) {
                const newLayout = event.data.split(",")[1];
                if (newLayout) {
                    root.evalNewKeyboardChange(newLayout);
                }
                return;
            }

            if (["workspace", "moveworkspace", "activespecial", "focusedmon"].includes(n)) {
                Hyprland.refreshWorkspaces();
                Hyprland.refreshMonitors();
            } else if (["openwindow", "closewindow", "movewindow"].includes(n)) {
                Hyprland.refreshToplevels();
                Hyprland.refreshWorkspaces();
            } else if (n.includes("workspace")) {
                Hyprland.refreshWorkspaces();
            } else if (n.includes("window") || n.includes("group") || ["pin", "fullscreen", "changefloatingmode", "minimize".includes(n)]) {
                Hyprland.refreshToplevels();
            } else if (n == "activelayout") {
                console.log("COCK");
                const newLayout = n.split(">>").at(1);
                if (newLayout) {
                    root.keyboardLayoutChanged(newLayout);
                }
            }
        }
    }

    HyprExtras {
        id: hyprExtras
        configPath: `${Paths.home}/.config/hypr`
        keyboardLayoutHandler: KeyboardService.keyboardLayoutHandler

        Component.onCompleted: {
            initConfigParse();
            updateCurrentKeyboardConfig();
        }
    }
}
