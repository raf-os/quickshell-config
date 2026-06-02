import MyShellControlPanel.plugin
import MyShellControlPanel.components
import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick
import QtQuick.Layouts

FormInput {
    id: root

    property color selectedColor: "black"

    Layout.fillWidth: true
    implicitHeight: layoutRoot.implicitHeight

    inputItem: adapter

    QtObject {
        id: adapter

        property color value: "black"
    }

    RowLayout {
        id: layoutRoot

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        Rectangle {
            id: colIndicator

            implicitWidth: 48
            implicitHeight: 48

            color: adapter.value

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
