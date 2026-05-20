import MyShellPlugin
import MyShellPlugin.Configs
import MyShellControlPanel.components
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    required property string name
    property string value
    property bool isDirty: false
    property bool isValid: true

    Layout.fillWidth: true
    implicitHeight: layoutRoot.implicitHeight

    ColumnLayout {
        id: layoutRoot

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        StyledText {
            id: label

            text: root.name

            Layout.fillWidth: true
            elide: Text.ElideRight
            leftPadding: Config.appearance.spacing.md

            font.pointSize: Config.appearance.fontSize.sm
            font.weight: 600
        }

        TextField {
            id: inputField

            activeFocusOnTab: true
            Layout.fillWidth: true
            padding: Config.appearance.padding.sm

            text: root.value
            color: Colors.colors.baseContent
            font.pointSize: Config.appearance.fontSize.sm

            onEditingFinished: {
                if (text !== root.value) {
                    root.isDirty = true;
                }
            }

            background: Rectangle {
                id: inputBg
                // anchors.fill: parent

                color: inputField.activeFocus ? Colors.colors.base : Colors.colors.base2
                radius: Config.appearance.rounding.sm

                border.width: 2
                border.color: "transparent"

                states: [
                    State {
                        name: "invalid"
                        when: !root.isValid
                        PropertyChanges {
                            inputBg.border.color: Colors.colors.destructive
                        }
                    },
                    State {
                        name: "active"
                        when: root.isValid && inputField.activeFocus
                        PropertyChanges {
                            inputBg.border.color: Colors.colors.primary
                        }
                    },
                    State {
                        name: "dirty"
                        when: root.isValid && root.isDirty
                        PropertyChanges {
                            inputBg.border.color: Colors.colors.base5
                        }
                    }
                ]

                Behavior on border.color {
                    CAnim {
                        duration: 100
                    }
                }
            }
        }
    }
}
