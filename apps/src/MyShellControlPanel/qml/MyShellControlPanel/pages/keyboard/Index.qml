pragma ComponentBehavior: Bound

import MyShellPlugin
import MyShellPlugin.Configs
import MyShellControlPanel.plugin
import MyShellControlPanel.components
import MyShellControlPanel.singletons
import QtQuick
import QtQuick.Layouts

PageStackItem {
    id: root
    title: "Keyboard"

    readonly property color fgCol: Colors.colors.base
    property bool hasChanges: settingsBuffer.isDirty

    KeyboardSettingsBuffer {
        id: settingsBuffer

        Component.onCompleted: {
            settingsBuffer.instance = Hypr.instance;
        }
    }

    ColumnLayout {
        id: layout

        anchors.fill: parent

        spacing: Config.appearance.spacing.sm

        ColumnLayout {
            Layout.fillWidth: true

            StyledText {
                text: "Installed layouts"

                font.pointSize: Config.appearance.fontSize.sm
            }

            ActiveLayoutList {
                id: activeLayoutList
                enabled: !Hypr.isKbSwitchOnCooldown

                settingsBuffer: settingsBuffer
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 256

            StyledText {
                text: "Available layouts"
                font.pointSize: Config.appearance.fontSize.sm
            }

            KeyboardList {
                id: allLayoutsList

                settingsBuffer: settingsBuffer
            }
        }

        RowLayout {
            Layout.fillWidth: true

            spacing: Config.appearance.spacing.md

            SButton {
                text: "Apply settings"
                autoWidth: true
                Layout.fillWidth: true
                disabled: !root.hasChanges

                onClicked: {
                    settingsBuffer.applyChanges();
                }
            }

            SButton {
                text: "Cancel changes"
                autoWidth: true
                Layout.fillWidth: true
                type: SButton.ButtonType.Destructive
                disabled: !root.hasChanges

                onClicked: {
                    settingsBuffer.resetForm();
                }
            }
        }
    }

    component FgWrapperBg: Rectangle {
        anchors.fill: parent
        radius: Config.appearance.rounding.sm
        color: root.fgCol
    }
}
