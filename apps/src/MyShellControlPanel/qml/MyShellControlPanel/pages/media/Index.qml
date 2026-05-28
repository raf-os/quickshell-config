pragma ComponentBehavior: Bound

import MyShellPlugin
import MyShellPlugin.Configs
import MyShellControlPanel.components
import MyShellControlPanel.components.forms
import MyShellControlPanel.plugin
import QtQuick
import QtQuick.Effects
import QtQuick.Layouts
import QtQuick.Controls

PageStackItem {
    id: root
    title: "Media"

    FormController {
        id: formController
        models: [Config.media] // qmllint disable missing-type
    }

    ColumnLayout {
        id: mainLayout

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        spacing: Config.appearance.spacing.md

        HeaderText {
            text: "Media Module"
            level: 1
        }

        FormPreset {
            id: mediaRootForm

            controller: formController
            innerSpacing: Config.appearance.spacing.sm

            delegate: AutoFieldDelegate {
                layoutFillWidth: true
            }
        }
    }
}
