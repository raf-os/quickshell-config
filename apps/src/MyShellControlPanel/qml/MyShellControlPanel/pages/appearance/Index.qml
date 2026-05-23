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

    FontValidator {
        id: fontValidator
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
            controller: fontsForm
            isDirty: fontsForm.isDirty
            innerSpacing: Config.appearance.spacing.sm

            delegate: SSelector {
                required property FieldController modelData
                controller: modelData

                name: modelData.name
                model: fontValidator.fontFamilies
            }
        }

        HorizontalSeparator {}

        SectionTitle {
            text: "UI Scaling"
        }

        FormPreset {
            id: formRangeFields

            layoutType: FormPreset.Type.Grid
            columns: 3

            gridSpacing: Qt.vector2d(Config.appearance.spacing.md, Config.appearance.spacing.sm)
            controller: miscAppearanceForm
            isDirty: miscAppearanceForm.isDirty
            delegate: AutoFieldDelegate {
                includeClassName: true
                layoutFillWidth: true
                floatMinimumValue: 0.0
                floatMaximumValue: 2.0
            }

            // delegate: SFloatInput {
            //     required property FieldController modelData
            //     controller: modelData
            //
            //     Layout.fillWidth: true
            //     boxLayoutFillWidth: true
            //     textInset: 0
            //
            //     stepSize: 0.1
            //     from: 0.1
            //     to: 2.0
            //
            //     name: `${modelData.className}.${modelData.name}`
            // }
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
