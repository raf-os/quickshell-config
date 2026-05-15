import MyShellControlPanel.components
import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick
import QtQuick.Layouts

Item {
    id: root

    required property string name
    property color selectedColor: "black"

    Layout.fillWidth: true
    implicitHeight: layoutRoot.implicitHeight

    RowLayout {
        id: layoutRoot

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        Rectangle {
            id: colIndicator

            implicitWidth: 48
            implicitHeight: 48

            color: root.selectedColor

            radius: Config.appearance.rounding.sm

            border.width: 1
            border.color: Colors.colors.baseContent
        }

        StyledText {
            id: label

            text: root.name

            Layout.fillWidth: true

            elide: Text.ElideRight
        }
    }
}
