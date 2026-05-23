import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick

Text {
    property real fill
    property int grade: 0

    renderType: Text.NativeRendering

    color: Colors.colors.baseContent
    font.family: Config.appearance.fontFamily.material
    // font.pixelSize: Config.appearance.fontSize.lg
    font.variableAxes: ({
            FILL: fill.toFixed(1),
            GRAD: grade,
            opsz: fontInfo.pixelSize,
            wght: fontInfo.weight
        })
}
