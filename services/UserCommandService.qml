pragma Singleton

import qs.utils
import MyShellPlugin
import Quickshell
import Quickshell.Io
import QtQuick

Singleton {
    id: root

    property bool isFirstLoad: true
    property string cmdQuery
    property alias entries: cmdHandler.entries

    function setQuery(newQuery: string): void {
        cmdQuery = newQuery;
    }

    function setup() {
        // Simply a way to not lazy load this entire singleton
        // The actual command entries will be lazy loaded though
        console.info("Starting up UserCommandService...");
    }

    function attemptFirstLoad() {
        if (!isFirstLoad)
            return;

        cmdHandler.refreshCommandList();
        isFirstLoad = false;
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
