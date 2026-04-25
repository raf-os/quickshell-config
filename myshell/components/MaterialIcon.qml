import qs.config
import qs.services
import QtQuick

Text {
    property real fill
    property int grade: 0

    color: ColorService.current.baseContent
    font.family: Config.appearance.fontFamily.material
    font.pointSize: Config.appearance.fontSize.lg
    font.variableAxes: ({
            FILL: fill.toFixed(1),
            GRAD: grade,
            opsz: fontInfo.pixelSize,
            wght: fontInfo.weight
        })
}
