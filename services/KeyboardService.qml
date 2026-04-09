pragma Singleton

import qs.config
import qs.utils
import MyShellPlugin
import Quickshell
import Quickshell.Io
import QtQuick

Singleton {
    id: root

    property alias keyboardLayoutHandler: kbLHandler

    property list<string> userLayouts: Config.keymap.layouts

    KeyboardLayoutHandler {
        id: kbLHandler

        // onLayoutsChanged: {
        //     debugPrintLayouts();
        // }
    }
}
