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
        id: fontsForm
        models: [Config.appearance.fontFamily] // qmllint disable missing-type
    }

    FormController {
        id: miscAppearanceForm
        models: [Config.appearance.fontSize, Config.appearance.rounding, Config.appearance.spacing, Config.appearance.padding] // qmllint disable missing-type
    }

    ColumnLayout {
        id: mainLayout

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        spacing: Config.appearance.spacing.md

        FormPreset {
            model: fontsForm.fields
            isDirty: fontsForm.isDirty

            delegate: STextInput {
                required property FieldController modelData

                onIsDirtyChanged: {
                    modelData.isDirty = isDirty;
                }

                name: modelData.name
                value: modelData.value
            }
        }

        HorizontalSeparator {}

        FormPreset {
            id: formRangeFields
            model: miscAppearanceForm.fields
            isDirty: miscAppearanceForm.isDirty
            delegate: SFloatInput {
                required property FieldController modelData

                onIsDirtyChanged: {
                    modelData.isDirty = isDirty;
                }

                function onValidate() {
                    const val = Number(value);
                    if (isNaN(val)) {
                        return {
                            success: false,
                            message: "Only numbers are allowed"
                        };
                    }
                }

                Layout.fillWidth: true
                boxLayoutFillWidth: true
                textInset: 0

                stepSize: 0.1
                from: 0.1
                to: 2.0

                name: `${modelData.className}.${modelData.name}`
                value: modelData.value
            }
            customContent: GridLayout {
                Layout.fillWidth: true
                columns: 3
                rowSpacing: Config.appearance.spacing.lg
                columnSpacing: Config.appearance.spacing.md

                Repeater {
                    model: formRangeFields.model
                    delegate: formRangeFields.delegate
                }
            }
        }
    }
}
