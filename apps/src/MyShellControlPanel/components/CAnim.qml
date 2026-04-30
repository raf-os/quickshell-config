import MyShellPlugin.Configs
import QtQuick

ColorAnimation {
    duration: 400
    easing.type: Easing.BezierSpline
    easing.bezierCurve: Config.appearance.animCurves.linear
}
