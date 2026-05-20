pragma ComponentBehavior: Bound

import MyShellPlugin
import MyShellPlugin.Configs
import MyShellControlPanel.components
import MyShellControlPanel.plugin
import QtQuick
import QtQuick.Layouts

ColumnLayout {
    id: root

    enum Type {
        Column = 0,
        Row = 1,
        Grid = 2
    }

    required property Component delegate
    required property list<FieldController> model
    property Component customContent: DefaultContent {}

    property bool isDirty: false

    Layout.fillWidth: true
    spacing: Config.appearance.spacing.md

    function onSave() {
    }

    function onCancel() {
    }

    Loader {
        Layout.fillWidth: true

        sourceComponent: root.customContent
    }

    RowLayout {
        id: actionButtons
        Layout.fillWidth: true

        spacing: Config.appearance.spacing.md

        SButton {
            id: saveButton
            text: "Save"
            autoWidth: true
            Layout.fillWidth: true
            disabled: !root.isDirty
        }

        SButton {
            id: cancelButton
            text: "Cancel"
            autoWidth: true
            Layout.fillWidth: true
            type: SButton.ButtonType.Destructive
            disabled: !root.isDirty
        }
    }

    component DefaultContent: ColumnLayout {
        spacing: root.spacing

        Repeater {
            model: root.model
            delegate: root.delegate
        }
    }
}
