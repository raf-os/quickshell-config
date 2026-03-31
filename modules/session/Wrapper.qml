pragma ComponentBehavior: Bound

import qs.components
import qs.config
import Quickshell
import QtQuick

Item {
    id: root

    required property PersistentProperties openPanels
    required property var panels
    readonly property real initialHeight: content.item ? content.implicitHeight : 0

    visible: height > 0
    clip: true
    implicitWidth: content.implicitWidth
    implicitHeight: openPanels.session ? initialHeight : 0

    Behavior on implicitHeight {
        NAnim {
            duration: 400
            easing.bezierCurve: Config.appearance.animCurves.defaultEase
        }
    }

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
