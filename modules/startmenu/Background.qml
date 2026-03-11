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
        relativeX: root.wrapper.width + root.roundingX
        relativeY: 0
    }

    PathArc {
        relativeX: -root.roundingX
        relativeY: root.rounding
        radiusX: Math.min(root.rounding, root.wrapper.width)
        radiusY: root.rounding
        direction: PathArc.Counterclockwise
    }

    PathLine {
        relativeX: 0
        relativeY: root.wrapper.height - root.rounding * 2
    }

    PathArc {
        relativeX: root.roundingX
        relativeY: root.rounding
        radiusX: Math.min(root.rounding, root.wrapper.width)
        radiusY: root.rounding
        direction: PathArc.Counterclockwise
    }

    PathLine {
        relativeX: -root.wrapper.width - root.roundingX
        relativeY: 0
    }

    PathLine {
        relativeX: 0
        relativeY: -root.wrapper.height
    }
}
