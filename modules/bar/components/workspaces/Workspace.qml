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

    readonly property bool isActive: modelData === activeWsId
    readonly property bool isOccupied: occupied[modelData] ?? false
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
            anchors.fill: parent
            radius: root.iconActiveSize / 2
            color: ColorService.current.primary
            blur: 12
            spread: 6
            opacity: indicator.shadowOpacity
        }
    }

    Rectangle {
        id: indicator

        property real shadowOpacity: 0

        anchors.centerIn: parent

        implicitWidth: root.isActive || mouseArea.containsMouse ? root.iconActiveSize : root.iconInactiveSize
        implicitHeight: root.isActive || mouseArea.containsMouse ? root.iconActiveSize : root.iconInactiveSize

        color: root.isActive ? ColorService.current.primary5 : root.isOccupied ? ColorService.current.primary : ColorService.current.base2
        radius: 1000

        states: State {
            name: "active"
            when: root.isActive

            PropertyChanges {
                indicator.shadowOpacity: 1
            }
        }

        transitions: [
            Transition {
                from: ""
                to: "active"

                NAnim {
                    target: indicator
                    property: "shadowOpacity"
                    easing.bezierCurve: Config.appearance.animCurves.defaultEase
                    duration: 300
                }
            },
            Transition {
                from: "active"
                to: ""

                NAnim {
                    target: indicator
                    property: "shadowOpacity"
                    easing.bezierCurve: Config.appearance.animCurves.defaultEase
                    duration: 300
                }
            }
        ]

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

        // layer.enabled: !GlobalStateManager.isGameMode
        // layer.effect: MultiEffect {
        //     blurMax: 20
        //     shadowEnabled: true
        //     shadowColor: "#e25016"
        //     shadowOpacity: indicator.shadowOpacity
        //     shadowScale: 1.5
        // }
    }
}
