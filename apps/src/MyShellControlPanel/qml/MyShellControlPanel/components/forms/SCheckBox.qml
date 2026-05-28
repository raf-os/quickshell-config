import org.nightshell.Components

import MyShellControlPanel.plugin
import MyShellControlPanel.components
import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick
import QtQuick.Layouts

FormInput {
    id: root

    property int boxSize: 24
    property int boxRadius: Config.appearance.rounding.sm

    Layout.fillWidth: true
    implicitHeight: layout.implicitHeight

    inputItem: control

    RowLayout {
        id: layout

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        spacing: Config.appearance.spacing.sm

        Item {
            id: control

            activeFocusOnTab: true

            // TODO: Something about the value below is not working properly when the C++ plugin compares this value to determine dirty status.
            // My guess is some sort of type shenanigans due to the usage of QVariant in the backend.
            // Try to find a solution.
            property bool value
            readonly property bool checked: value === true
            readonly property bool hover: interactionArea.containsMouse

            implicitWidth: root.boxSize
            implicitHeight: root.boxSize

            function toggle() {
                if (enabled) {
                    value = !value;
                }
            }

            Keys.onReturnPressed: ev => {
                control.toggle();
            }

            Rectangle {
                id: controlRect
                anchors.fill: parent

                color: control.hover ? Colors.colors.base3 : Colors.colors.base2

                border.width: 2
                border.color: "transparent"

                radius: root.boxRadius

                states: [
                    State {
                        name: "invalid" // How would this even happen?
                        when: root.isValid === false
                        PropertyChanges {
                            controlRect.border.color: Colors.colors.destructive
                        }
                    },
                    State {
                        name: "active"
                        when: root.isValid && control.activeFocus
                        PropertyChanges {
                            controlRect.border.color: Colors.colors.primary
                        }
                    },
                    State {
                        name: "dirty"
                        when: root.isValid && root.isDirty
                        PropertyChanges {
                            controlRect.border.color: Colors.colors.base5
                        }
                    }
                ]

                Behavior on border.color {
                    CAnim {
                        duration: 100
                    }
                }
            }

            SVGIcon {
                id: checkIcon

                anchors.centerIn: parent
                iconName: "check"

                size: root.boxSize * 0.8
                opacity: control.checked ? 1 : 0
                scale: control.checked ? 1 : 0
                visible: opacity > 0

                Behavior on opacity {
                    NAnim {
                        duration: 200
                        easing.bezierCurve: Config.appearance.animCurves.linear
                    }
                }

                Behavior on scale {
                    NAnim {
                        duration: 200
                    }
                }
            }
        }

        StyledText {
            id: label

            Layout.fillWidth: true

            text: root.name

            font.pointSize: Config.appearance.fontSize.sm
            font.family: Config.appearance.fontFamily.sans
            // font.weight: 600
        }
    }

    MouseArea {
        id: interactionArea

        anchors.fill: parent
        cursorShape: control.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
        hoverEnabled: true

        onClicked: {
            control.toggle();

            if (control.enabled)
                control.forceActiveFocus();
        }
    }
}
