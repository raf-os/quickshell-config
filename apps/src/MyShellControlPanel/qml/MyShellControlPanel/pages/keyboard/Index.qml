pragma ComponentBehavior: Bound

import MyShellPlugin
import MyShellPlugin.Configs
import MyShellControlPanel.components
import MyShellControlPanel.singletons
import QtQuick
import QtQuick.Layouts

PageStackItem {
    id: root
    title: "Keyboard"

    readonly property color fgCol: Colors.colors.base
    property bool hasChanges: false

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

            Item {
                id: lvWrapper

                readonly property int padding: Config.appearance.padding.sm

                Layout.fillWidth: true
                Layout.margins: padding
                implicitHeight: 128

                FgWrapperBg {
                    id: lvWrapperBg
                }

                ListView {
                    id: layoutSelectList
                    model: Hyprland.inputLayouts

                    anchors.fill: parent
                    anchors.margins: lvWrapper.padding
                    clip: true
                    focus: true

                    activeFocusOnTab: true

                    spacing: Config.appearance.padding.xs

                    delegate: Item {
                        id: kbLayout
                        required property HyprKeyboardLayout modelData
                        readonly property bool isActive: Hyprland.currentLayout?.layout === modelData?.layout ?? false

                        implicitWidth: ListView.view ? ListView.view.width : 0
                        implicitHeight: layoutName.height

                        Rectangle {
                            anchors.left: parent.left
                            anchors.top: parent.top
                            implicitWidth: kbLayout.isActive ? parent.implicitWidth : 0
                            implicitHeight: parent.implicitHeight

                            color: Colors.colors.base4
                            radius: Config.appearance.rounding.sm
                            opacity: kbLayout.isActive ? 1 : 0

                            Behavior on implicitWidth {
                                NAnim {}
                            }

                            Behavior on opacity {
                                NAnim {}
                            }
                        }

                        StyledText {
                            id: layoutName
                            text: kbLayout.modelData?.description ?? ""
                            font.pointSize: Config.appearance.fontSize.sm
                            font.weight: kbLayout.isActive ? 600 : 500

                            padding: Config.appearance.padding.xxs

                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }
                }
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

            KeyboardList {}
        }

        RowLayout {
            Layout.fillWidth: true

            spacing: Config.appearance.spacing.md

            SButton {
                text: "Apply settings"
                autoWidth: true
                Layout.fillWidth: true
                disabled: !root.hasChanges
            }

            SButton {
                text: "Cancel changes"
                autoWidth: true
                Layout.fillWidth: true
                type: SButton.ButtonType.Destructive
                disabled: !root.hasChanges
            }
        }
    }

    component FgWrapperBg: Rectangle {
        anchors.fill: parent
        radius: Config.appearance.rounding.sm
        color: root.fgCol
    }
}
