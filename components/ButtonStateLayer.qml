import qs.services
import qs.config
import qs.components
import QtQuick

MouseArea {
    id: root

    property bool disabled
    property bool showHoverBackground: true
    property color color: "white"
    property real radius: parent?.radius ?? 0

    function onClicked(): void {
    }

    anchors.fill: parent

    enabled: !disabled
    cursorShape: disabled ? undefined : Qt.PointingHandCursor
    hoverEnabled: true

    onClicked: event => !disabled && onClicked(event)

    Rectangle {
        id: hoverLayer

        anchors.fill: parent
        radius: root.radius

        color: Qt.alpha(root.color, root.disabled ? 0 : root.pressed ? 1 : (root.showHoverBackground && root.containsMouse) ? 0.5 : 0)
        scale: root.containsMouse ? 1 : 0.5

        Behavior on scale {
            NAnim {
                easing.bezierCurve: Config.appearance.animCurves.accelerateOverCorrect
                duration: 400
            }
        }

        Behavior on color {
            CAnim {
                duration: 150
            }
        }
    }
}
