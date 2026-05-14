pragma Singleton

import MyShellPlugin as MSP
import MyShellPlugin.Utils as MSPUT
import QtQuick

Item {
    id: root

    property alias handler: handler

    MSP.KeyboardLayoutHandler {
        id: handler

        cachePath: MSPUT.Paths.cache
    }
}
