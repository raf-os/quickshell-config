pragma Singleton

import qs.utils
import qs.modules
import MyShellPlugin
import Quickshell
import Quickshell.Io
import QtQuick

Singleton {
    id: root

    property bool isFirstLoad: true
    property string cmdQuery
    property alias entries: cmdHandler.entries
    property alias cmdHandler: cmdHandler

    function setQuery(newQuery: string): void {
        cmdQuery = newQuery;
    }

    function setup() {
        // Simply a way to not lazy load this entire singleton
        // The actual command entries will be lazy loaded though
        console.info("Starting up UserCommandService...");
    }

    function showCapturePopup() {
        capturePopup.visible = true;
    }

    function executeCommand(command: string): var {
        const msg = cmdHandler.executeCommand(command);
        const isSuccess = msg["success"] === true ?? false;
        const captureOutput = msg["captureOutput"] === true ?? false;

        if (capturePopup) {
            showCapturePopup();
        }

        return {
            success: isSuccess,
            message: msg["message"] ?? "",
            captureOutput: captureOutput
        };
    }

    function attemptFirstLoad() {
        if (!isFirstLoad)
            return;

        cmdHandler.refreshCommandList();
        isFirstLoad = false;
    }

    CommandCaptureWindow {
        id: capturePopup
    }

    IpcHandler {
        target: "commands"

        function refreshCommandList(): string {
            cmdHandler.refreshCommandList();
            root.isFirstLoad = false;
            return "ok";
        }
    }

    CmdHandler {
        id: cmdHandler

        basePath: `${Quickshell.shellPath("modules/startmenu/commands")}`
        path: `${Paths.config}/commands`
        queryString: root.cmdQuery
    }
}
