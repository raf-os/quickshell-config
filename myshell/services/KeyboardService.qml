pragma Singleton

import qs.utils
import MyShellPlugin
import Quickshell
import Quickshell.Io
import QtQuick

Singleton {
    id: root

    property alias keyboardLayoutHandler: kbLHandler

    KeyboardLayoutHandler {
        id: kbLHandler

        cachePath: Paths.cache

        // onLayoutsChanged: {
        //     debugPrintLayouts();
        // }
    }
}
