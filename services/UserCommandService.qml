pragma Singleton

import qs.utils
import MyShellPlugin
import Quickshell
import QtQuick

Singleton {
    id: root

    property string cmdQuery
    property alias entries: cmdHandler.entries

    function setQuery(newQuery: string): void {
        cmdQuery = newQuery;
    }

    CmdHandler {
        id: cmdHandler

        basePath: `${Quickshell.shellPath("modules/startmenu/commands")}`
        path: `${Paths.config}/commands`
        queryString: root.cmdQuery

        Component.onCompleted: {
            refreshCommandList();
        }
    }
}
