pragma ComponentBehavior: Bound

import qs.components
import qs.services
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
    readonly property int iconActiveSize: 14
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

    StyledRect {
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
                }
            },
            Transition {
                from: "active"
                to: ""

                NAnim {
                    target: indicator
                    property: "shadowOpacity"
                }
            }
        ]

        layer.enabled: true
        layer.effect: MultiEffect {
            blurMax: 12
            shadowEnabled: true
            shadowColor: "#e25016"
            shadowOpacity: indicator.shadowOpacity
            shadowScale: 1.5
        }
    }
}
