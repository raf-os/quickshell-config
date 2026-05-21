pragma ComponentBehavior: Bound

import MyShellPlugin
import MyShellPlugin.Configs
import MyShellControlPanel.components
import MyShellControlPanel.components.forms
import MyShellControlPanel.plugin
import MyShellControlPanel.plugin.validators
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

        SectionTitle {
            text: "Fonts"
        }

        FormPreset {
            // model: fontsForm.fields
            controller: fontsForm
            isDirty: fontsForm.isDirty

            delegate: SSelector {
                required property FieldController modelData

                Component.onCompleted: {
                    modelData.cValidator = fontValidator;
                }

                onIsDirtyChanged: {
                    modelData.isDirty = isDirty;
                }

                name: modelData.name
                value: modelData.value
                model: fontValidator.fontFamilies

                FontValidator {
                    id: fontValidator
                }
            }
        }

        HorizontalSeparator {}

        SectionTitle {
            text: "UI Scaling"
        }

        FormPreset {
            id: formRangeFields
            // model: miscAppearanceForm.fields
            controller: miscAppearanceForm
            isDirty: miscAppearanceForm.isDirty
            delegate: SFloatInput {
                required property FieldController modelData
                model: modelData

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
                initialValue: modelData.initialValue
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

    component SectionTitle: Item {
        id: secTitComp
        required property string text

        Layout.fillWidth: true
        implicitHeight: secTitText.height

        StyledText {
            id: secTitText

            text: secTitComp.text
            font.family: Config.appearance.fontFamily.sans
            font.pointSize: Config.appearance.fontSize.lg
            font.weight: 600

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
