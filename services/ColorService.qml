pragma Singleton
pragma ComponentBehavior: Bound

import qs.config
import qs.utils
import Quickshell
import Quickshell.Io
import QtQuick

Singleton {
    id: root

    readonly property ColorPalette current: ColorPalette {}

    component ColorPalette: QtObject {
        property color base0: "#1b1510"
        property color base: "#25201d"
        property color base2: "#322d28"
        property color base3: "#4a3d34"
        property color base4: "#776156"

        property color baseContent: "#d9d3cd"

        property color primary: "#e25016"
        property color primary5: "#f4b17a"

        property color destructive: "#e32e35"
        property color destructiveHover: "#f87767"
    }
}
