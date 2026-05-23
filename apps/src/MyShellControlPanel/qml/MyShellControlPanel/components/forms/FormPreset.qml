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
    required property FormController controller
    property list<FieldController> model: controller.fields
    property int layoutType: FormPreset.Type.Column
    property int innerSpacing: 0
    property vector2d gridSpacing: Qt.vector2d(0, 0)
    property int rows: 0
    property int columns: 0
    property alias layout: layout

    property bool isDirty: controller ? controller.isDirty : false

    Layout.fillWidth: true
    spacing: Config.appearance.spacing.md

    function onSave() {
    }

    function onCancel() {
        root.controller.resetForm();
    }

    GridLayout {
        id: layout

        states: [
            State {
                when: root.layoutType === FormPreset.Type.Grid
                PropertyChanges {
                    layout.rows: root.rows
                    layout.columns: root.columns
                    layout.columnSpacing: root.gridSpacing.x
                    layout.rowSpacing: root.gridSpacing.y
                }
            },
            State {
                when: root.layoutType === FormPreset.Type.Column
                PropertyChanges {
                    layout.columns: 1
                    layout.rowSpacing: root.innerSpacing
                }
            },
            State {
                when: root.layoutType === FormPreset.Type.Row
                PropertyChanges {
                    layout.rows: 1
                    layout.columnSpacing: root.innerSpacing
                }
            }
        ]

        Repeater {
            model: root.model
            delegate: root.delegate
        }
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

            onClicked: {
                if (disabled)
                    return;
                root.onCancel();
            }
        }
    }
}
