pragma ComponentBehavior: Bound

import qs.config
import qs.components
import Quickshell
import QtQuick

Item {
    id: root

    required property PersistentProperties openPanels

    readonly property bool isEnabled: Config.media.mediaPopoutEnabled
    readonly property real initialHeight: content.item ? (content.implicitHeight + padding * 2) : 0
    readonly property int padding: Config.appearance.padding.sm

    visible: isEnabled && height > 0
    clip: true
    implicitWidth: content.implicitWidth + padding * 2
    implicitHeight: (isEnabled && openPanels.mprisViewer) ? initialHeight : 0

    Behavior on implicitHeight {
        NAnim {
            duration: 400
            easing.bezierCurve: Config.appearance.animCurves.defaultEase
        }
    }

    Loader {
        id: content

        anchors.bottom: parent.bottom
        anchors.bottomMargin: root.padding
        anchors.horizontalCenter: parent.horizontalCenter

        Component.onCompleted: {
            active = Qt.binding(() => root.openPanels.mprisViewer || root.visible);
        }

        sourceComponent: MprisContent {
            openPanels: root.openPanels
        }
    }
}
