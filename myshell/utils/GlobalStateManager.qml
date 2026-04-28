pragma Singleton

import qs.utils
import MyShellPlugin.Utils
import Quickshell
import Quickshell.Io
import QtQuick

Singleton {
    id: root
    property bool isGameMode: false

    function checkGamemodeState(callback: var) {
        if (actionCooldownTimer.running)
            return;
        checkStateProcess.running = true;
        if (callback !== null && typeof callback === "function")
            checkStateProcess.callback = callback;
        actionCooldownTimer.start();
    }

    function toggleGamemodeState() {
        checkGamemodeState(() => {
            root.isGameMode = !root.isGameMode;
            Quickshell.execDetached(["sh", "-c", `echo ${root.isGameMode ? "true" : "false"} > ${Paths.state}/gamemode`]);
        });
    }

    function syncHyprlandGamemode(): void {
        if (!root.isGameMode) {
            Quickshell.execDetached(["hyprctl", "reload"]);
        } else {
            Quickshell.execDetached(["hyprctl", "--batch", "keyword animations:enabled 0; " + "keyword decoration:shadow:enabled 0; " + "keyword decoration:blur:enabled 0; " + "keyword decoration:fullscreen_opacity 1; " + "keyword general:gaps_in 0; " + "keyword general:border_size 1; " + "keyword decoration:rounding 0"]);
        }
    }

    Timer {
        id: actionCooldownTimer

        interval: 250
    }

    Process {
        id: checkStateProcess
        property var callback: null

        running: true
        command: ["cat", `${Paths.state}/gamemode`]
        stdout: StdioCollector {
            onStreamFinished: {
                if (this.text.trim() === "true") {
                    root.isGameMode = true;
                } else {
                    root.isGameMode = false;
                }
                if (checkStateProcess.callback != null) {
                    if (typeof checkStateProcess.callback !== "function") {
                        checkStateProcess.callback = null;
                        return;
                    }
                    checkStateProcess.callback?.();
                    checkStateProcess.callback = null;
                }
                root.syncHyprlandGamemode();
            }
        }
    }
}
