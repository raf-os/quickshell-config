import qs.config
import qs.components
import QtQuick

Item {
    id: root
    property string text: ""
    implicitWidth: Config.appearance.fontSize.xl
    implicitHeight: Config.bar.sizes.innerHeight

    StyledText {
        id: icon
        anchors.centerIn: parent
        text: root.text
        font.pixelSize: Config.appearance.fontSize.xl
        font.family: Config.appearance.fontFamily.monoIcon
    }
}
