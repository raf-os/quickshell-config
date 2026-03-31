import qs.components
import qs.config
import qs.services
import QtQuick

MouseArea {
    id: root

    required property bool isEnabled
    required property string iconText
    property int size: 24

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

        color: root.isEnabled ? ColorService.current.baseContent : ColorService.current.baseContentMuted
        font.pixelSize: root.size

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
