import MyShellPlugin
import MyShellPlugin.Configs
import MyShellControlPanel.components
import MyShellControlPanel.plugin
import QtQuick
import QtQuick.Layouts

FormInput {
    id: root

    property int spacing: Config.appearance.spacing.xxs
    property int textInset: Config.appearance.spacing.sm
    property bool boxLayoutFillWidth: true

    Layout.fillWidth: true
    implicitHeight: layoutRoot.implicitHeight

    inputItem: control

    ColumnLayout {
        id: layoutRoot

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        spacing: root.spacing

        StyledText {
            id: label

            text: root.name

            Layout.fillWidth: true
            elide: Text.ElideRight
            leftPadding: Config.appearance.spacing.md

            font.pointSize: Config.appearance.fontSize.sm
            font.weight: 600
        }

        STextField {
            id: control

            activeFocusOnTab: true
            Layout.fillWidth: true

            isDirty: root.isDirty
            isValid: root.isValid
        }
    }
}
