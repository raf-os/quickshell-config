pragma ComponentBehavior: Bound

import MyShellPlugin
import MyShellPlugin.Configs
import MyShellControlPanel.components
import MyShellControlPanel.components.forms
import MyShellControlPanel.plugin
import QtQuick
import QtQuick.Layouts

PageStackItem {
    id: root
    title: "Appearance"

    FormController {
        id: formController
        model: Config.appearance.fontFamily // qmllint disable missing-type
    }

    ColumnLayout {
        id: mainLayout

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        spacing: Config.appearance.spacing.md

        Repeater {
            model: formController.fields

            delegate: STextInput {
                required property FieldController modelData

                name: modelData.name
                value: modelData.value
            }
        }
    }
}
