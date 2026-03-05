import qs.services
import qs.config
import QtQuick
import QtQuick.Shapes

ShapePath {
    id: root

    required property Wrapper wrapper

    readonly property real rounding: Config.border.rounding
    readonly property bool flatten: wrapper.width < rounding * 2
    readonly property real roundingX: flatten ? wrapper.width / 2 : rounding

    fillColor: ColorService.current.base
    strokeWidth: 0

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
}
