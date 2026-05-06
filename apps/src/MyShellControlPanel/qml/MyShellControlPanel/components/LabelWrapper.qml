pragma ComponentBehavior: Bound

import MyShellPlugin
import MyShellPlugin.Configs
import MyShellControlPanel.components
import QtQuick
import QtQuick.Layouts

ColumnLayout {
    id: root

    property string text: ""
    property alias label: labelText

    StyledText {
        id: labelText

        text: root.text

        Layout.fillWidth: true

        font.family: Config.appearance.fontFamily.sans
        font.pointSize: Config.appearance.fontSize.sm
    }
}
