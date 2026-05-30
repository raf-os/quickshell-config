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
    title: "Launcher"

    FormController {
        id: formController
        models: [Config.launcher] // qmllint disable missing-type
    }

    ColumnLayout {
        id: mainLayout

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        spacing: Config.appearance.spacing.md

        HeaderText {
            text: "App Launcher Settings"
            level: 1
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: Config.appearance.spacing.sm

            SCheckBox {
                id: enabledField

                name: "Enable launcher"

                Layout.fillWidth: true

                AutoFormField {
                    controller: formController
                    field: ["LauncherConfig", "enabled"]
                }
            }

            SIntegerInput {
                id: widthField

                name: "Launcher width"

                Layout.fillWidth: true

                from: 0
                to: 10000

                AutoFormField {
                    controller: formController
                    field: ["LauncherConfig", "width"]
                }
            }

            SStringInput {
                id: prefixField

                name: "Command prefix"

                Layout.fillWidth: true

                AutoFormField {
                    controller: formController
                    field: ["LauncherConfig", "commandPrefix"]
                }
            }

            FormButtons {
                id: launcherRootFormButtons

                controller: formController
            }
        }
    }
}
