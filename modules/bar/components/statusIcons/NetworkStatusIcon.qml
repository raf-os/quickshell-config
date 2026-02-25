import qs.config
import qs.components
import QtQuick

Item {
    id: root
    property string text: ""
    implicitWidth: icon.implicitWidth
    implicitHeight: icon.implicitHeight
    StyledText {
        id: icon
        text: root.text
        font.pointSize: Config.appearance.fontSize.md
    }
}
