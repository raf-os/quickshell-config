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
    title: "Bar Config"

    FormController {
        id: sizesConfigController
        models: [Config.bar.sizes] // qmllint disable missing-type
    }

    FormController {
        id: borderConfigController
        models: [Config.border] // qmllint disable missing-type
    }

    ColumnLayout {
        id: mainLayout

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        spacing: Config.appearance.spacing.md

        FormPreset {
            id: sizesForm

            controller: sizesConfigController
            innerSpacing: Config.appearance.spacing.xxs
            delegate: AutoFieldDelegate {
                layoutFillWidth: true
            }
        }

        HorizontalSeparator {}

        HeaderText {
            text: "Border Settings"
            level: 2
        }

        FormPreset {
            id: borderForm

            controller: borderConfigController
            innerSpacing: Config.appearance.spacing.xxs
            delegate: AutoFieldDelegate {
                layoutFillWidth: true
            }
        }
    }
}
