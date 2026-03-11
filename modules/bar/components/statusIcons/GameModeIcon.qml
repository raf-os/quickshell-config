pragma ComponentBehavior: Bound

import qs.components
import qs.config
import qs.services
import qs.utils
import QtQuick
import QtQuick.Effects

Item {
    id: root

    property bool isActive: GlobalStateManager.isGameMode
    property real shadowOpacity: 0
    readonly property int animDuration: 200

    implicitWidth: Config.appearance.fontSize.lg * 1.2
    implicitHeight: Config.appearance.fontSize.lg * 1.2

    function checkGamemodeState(callback: var): void {
        GlobalStateManager.checkGamemodeState(callback);
    }

    states: State {
        name: "active"
        when: root.isActive

        PropertyChanges {
            root.shadowOpacity: 1
        }
    }

    transitions: [
        Transition {
            from: ""
            to: "active"

            NAnim {
                target: root
                property: "shadowOpacity"
                easing.bezierCurve: Config.appearance.animCurves.linear
                duration: root.animDuration
            }
        },
        Transition {
            from: "active"
            to: ""

            NAnim {
                target: root
                property: "shadowOpacity"
                easing.bezierCurve: Config.appearance.animCurves.linear
                duration: root.animDuration
            }
        }
    ]

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        acceptedButtons: Qt.LeftButton

        onClicked: event => {
            if (event.button === Qt.LeftButton) {
                GlobalStateManager.toggleGamemodeState();
            }
        }
    }

    StyledText {
        id: icon
        anchors.centerIn: parent
        text: "󰊴"
        color: root.isActive ? ColorService.current.primary : ColorService.current.baseContent

        font.pixelSize: Config.bar.sizes.innerHeight * 1.25
        font.family: Config.appearance.fontFamily.monoIcon

        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: ColorService.current.primary
            shadowOpacity: root.shadowOpacity
            blurMax: 16
        }

        Behavior on color {
            CAnim {
                duration: root.animDuration
            }
        }
    }
}
