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
    property alias buttons: actionButtons

    property bool isDirty: controller.isDirty ?? false

    Layout.fillWidth: true
    spacing: Config.appearance.spacing.md

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

    FormButtons {
        id: actionButtons
        Layout.fillWidth: true

        controller: root.controller
    }
}
