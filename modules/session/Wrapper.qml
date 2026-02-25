pragma ComponentBehavior: Bound

import qs.components
import qs.config
import Quickshell
import QtQuick

Item {
    id: root

    required property PersistentProperties openPanels
    required property var panels
    readonly property real initialHeight: content.implicitHeight

    visible: height > 0
    clip: true
    implicitWidth: content.implicitWidth
    implicitHeight: 0

    states: State {
        name: "visible"
        when: root.openPanels.session

        PropertyChanges {
            root.implicitHeight: root.initialHeight
        }
    }

    transitions: [
        Transition {
            from: ""
            to: "visible"

            NAnim {
                target: root
                property: "implicitHeight"
                easing.bezierCurve: [0.38, 1.21, 0.22, 1, 1, 1]
            }
        },
        Transition {
            from: "visible"
            to: ""

            NAnim {
                target: root
                property: "implicitHeight"
                easing.bezierCurve: [0.38, 1.21, 0.22, 1, 1, 1]
            }
        }
    ]

    Loader {
        id: content

        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        Component.onCompleted: {
            active = Qt.binding(() => (root.openPanels.session || root.visible));
        }

        sourceComponent: Content {
            openPanels: root.openPanels
        }
    }
}
