pragma ComponentBehavior: Bound

import qs.config
import qs.components
import qs.services
import qs.utils
import Quickshell.Hyprland
import QtQuick
import QtQuick.Layouts
import QtQuick.Effects

Item {
    id: root

    required property int index
    required property int activeWsId
    required property int modelData
    required property var occupied
    required property var urgent

    readonly property bool isActive: modelData === activeWsId
    readonly property bool isOccupied: occupied[modelData] ?? false
    readonly property bool isUrgent: urgent[modelData] ?? false
    readonly property int iconActiveSize: 16
    readonly property int iconInactiveSize: 8

    Layout.alignment: Qt.AlignVCenter

    implicitWidth: iconActiveSize
    implicitHeight: iconActiveSize

    MouseArea {
        id: mouseArea

        hoverEnabled: true

        anchors.fill: parent

        cursorShape: Qt.PointingHandCursor

        onClicked: event => {
            Hyprland.dispatch(`workspace ${root.modelData}`);
        }
    }

    Loader {
        anchors.fill: parent
        active: !GlobalStateManager.isGameMode
        sourceComponent: RectangularShadow {
            id: glowLayer
            property real shadowOpacity: 0
            anchors.fill: parent
            radius: root.iconActiveSize / 2
            color: root.isUrgent ? ColorService.current.destructive : ColorService.current.primary
            blur: 12
            spread: 6
            opacity: shadowOpacity

            states: [
                State {
                    name: "active"
                    when: root.isActive && !root.isUrgent
                },
                State {
                    name: "urgent"
                    when: root.isUrgent
                }
            ]

            transitions: [
                Transition {
                    from: ""
                    to: "active"

                    NAnim {
                        target: glowLayer
                        property: "shadowOpacity"
                        easing.bezierCurve: Config.appearance.animCurves.defaultEase
                        duration: 300
                        to: 1
                    }
                },
                Transition {
                    // from: "active"
                    to: ""

                    NAnim {
                        target: glowLayer
                        property: "shadowOpacity"
                        easing.bezierCurve: Config.appearance.animCurves.defaultEase
                        duration: 300
                        to: 0
                    }
                },
                Transition {
                    to: "urgent"

                    SequentialAnimation {
                        loops: Animation.Infinite

                        NumberAnimation {
                            target: glowLayer
                            property: "shadowOpacity"
                            to: 1
                            duration: 100
                        }
                        NumberAnimation {
                            target: glowLayer
                            property: "shadowOpacity"
                            to: 0
                            duration: 2000
                            easing.type: Easing.BezierSpline
                            easing.bezierCurve: Config.appearance.animCurves.defaultEase
                        }
                    }
                }
            ]
        }
    }

    Rectangle {
        id: indicator

        property real shadowOpacity: 0

        anchors.centerIn: parent

        implicitWidth: root.isOccupied || root.isActive || mouseArea.containsMouse ? root.iconActiveSize : root.iconInactiveSize
        implicitHeight: root.isActive || mouseArea.containsMouse ? root.iconActiveSize : root.iconInactiveSize

        color: root.isUrgent ? ColorService.current.destructiveHover : root.isActive ? ColorService.current.primary5 : root.isOccupied ? ColorService.current.primary : ColorService.current.base3
        radius: 1000

        Behavior on color {
            CAnim {
                duration: 300
            }
        }

        Behavior on implicitWidth {
            NAnim {
                easing.bezierCurve: Config.appearance.animCurves.defaultEase
                duration: 300
            }
        }

        Behavior on implicitHeight {
            NAnim {
                easing.bezierCurve: Config.appearance.animCurves.defaultEase
                duration: 300
            }
        }
    }
}
