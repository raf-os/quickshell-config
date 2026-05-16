import MyShellPlugin
import MyShellPlugin.Configs
import MyShellControlPanel.components
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    required property string name
    property string value

    Layout.fillWidth: true
    implicitHeight: layoutRoot.implicitHeight

    ColumnLayout {
        id: layoutRoot

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        StyledText {
            id: label

            text: root.name

            Layout.fillWidth: true
            elide: Text.ElideRight
            leftPadding: Config.appearance.spacing.md

            font.pointSize: Config.appearance.fontSize.sm
            font.weight: 600
        }

        TextField {
            id: inputField

            activeFocusOnTab: true
            Layout.fillWidth: true
            padding: Config.appearance.padding.sm

            text: root.value
            color: Colors.colors.baseContent
            font.pointSize: Config.appearance.fontSize.sm

            background: Rectangle {
                // anchors.fill: parent

                color: inputField.activeFocus ? Colors.colors.base : Colors.colors.base2
                radius: Config.appearance.rounding.sm

                border.width: 2
                border.color: inputField.activeFocus ? Colors.colors.primary : "transparent"
            }
        }
    }
}
