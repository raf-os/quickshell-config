import qs.services
import qs.config
import QtQuick

MouseArea {
    id: root

    property bool disabled
    property bool showHoverBackground: true
    property color color: "red"
    property real radius: parent?.radius ?? 0

    function onClicked(): void {
    }

    anchors.fill: parent

    enabled: !disabled
    cursorShape: disabled ? undefined : Qt.PointingHandCursor
    hoverEnabled: true

    onClicked: event => !disabled && onClicked(event)

    StyledRect {
        id: hoverLayer

        anchors.fill: parent
        radius: root.radius

        color: Qt.alpha(root.color, root.disabled ? 0 : root.pressed ? 0.50 : (root.showHoverBackground && root.containsMouse) ? 0.25 : 0)
    }
}
