import qs.components
import qs.services
import qs.config
import QtQuick
import QtQuick.Shapes

ShapePath {
    id: root

    required property PopoutWrapper wrapper

    readonly property real rounding: Config.border.rounding
    readonly property bool flatten: wrapper.height < rounding * 2
    readonly property real roundingY: flatten ? wrapper.height / 2 : rounding

    strokeWidth: -1
    fillColor: ColorService.current.base

    PathLine {
        relativeX: root.wrapper.width
        relativeY: 0
    }

    PathLine {
        relativeX: 0
        relativeY: root.wrapper.height
    }

    PathLine {
        relativeX: -root.wrapper.width
        relativeY: 0
    }

    PathLine {
        relativeX: 0
        relativeY: -root.wrapper.height
    }

    Behavior on fillColor {
        CAnim {}
    }
}
