import MyShellPlugin.Configs
import QtQuick

NumberAnimation {
    duration: 400
    easing.type: Easing.BezierSpline
    easing.bezierCurve: Config.appearance.animCurves.defaultEase
}
