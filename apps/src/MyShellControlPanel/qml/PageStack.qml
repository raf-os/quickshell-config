pragma ComponentBehavior: Bound

import MyShellPlugin.Configs
import MyShellControlPanel.components
import QtQuick
import QtQuick.Controls

import "configs" as RootPage

StackView {
    id: root

    readonly property int animDuration: 300

    clip: true

    initialItem: RootPage.Index {}

    readonly property PageStackItem currentPage: currentItem as PageStackItem

    pushEnter: EnterAnim {}

    pushExit: ExitAnim {}

    popEnter: EnterAnim {}

    popExit: ExitAnim {}

    component EnterAnim: Transition {
        ParallelAnimation {
            PropertyAnimation {
                property: "opacity"
                from: 0
                to: 1
                duration: root.animDuration * 0.75
            }
            PropertyAnimation {
                property: "x"
                from: 128
                to: 0
                duration: root.animDuration
                easing.type: Easing.BezierSpline
                easing.bezierCurve: Config.appearance.animCurves.defaultEase
            }
        }
    }

    component ExitAnim: Transition {
        ParallelAnimation {
            PropertyAnimation {
                property: "opacity"
                from: 1
                to: 0
                duration: root.animDuration * 0.75
            }
            PropertyAnimation {
                property: "x"
                from: 0
                to: -128
                duration: root.animDuration
                easing.type: Easing.BezierSpline
                easing.bezierCurve: Config.appearance.animCurves.defaultEase
            }
        }
    }
}
