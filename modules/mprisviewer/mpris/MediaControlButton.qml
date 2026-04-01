import qs.components
import qs.config
import qs.services
import QtQuick

MouseArea {
    id: root

    required property bool isEnabled
    required property string iconText
    readonly property int size: 24
    property bool isFaded: false

    implicitWidth: size
    implicitHeight: size

    enabled: isEnabled
    acceptedButtons: Qt.LeftButton
    cursorShape: Qt.PointingHandCursor

    onClicked: event => clickAction(event)

    function clickAction(event: MouseEvent): void {
    }

    StyledText {
        id: icon

        anchors.fill: parent
        text: root.iconText

        color: root.isEnabled && !root.isFaded ? ColorService.current.baseContent : ColorService.current.baseContentMuted
        font.pixelSize: root.size
        font.family: Config.appearance.fontFamily.mono

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        Behavior on color {
            CAnim {
                duration: 300
                easing.bezierCurve: Config.appearance.animCurves.defaultEase
            }
        }
    }
}
