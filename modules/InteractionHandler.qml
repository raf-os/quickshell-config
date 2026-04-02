pragma ComponentBehavior: Bound

import "bar"
import "bar/popouts"
import qs.config
import Quickshell
import QtQuick

MouseArea {
    id: root

    required property ShellScreen screen
    required property PersistentProperties openPanels
    required property PopoutWrapper popouts
    required property BarWrapper bar
    required property Panels panels

    readonly property Item mediaIndicator: bar.mediaInfo ?? null

    propagateComposedEvents: true

    // acceptedButtons: Qt.LeftButton | Qt.RightButton
    anchors.fill: parent

    onClicked: event => {
        if (event.y < Config.border.thickness * 2 + Config.bar.sizes.innerHeight) {
            popouts.close();
            panels.forceClosePanels();
        }
    }

    onWheel: event => {
        bar.handleMouseWheel(event.x, event.angleDelta);
    }

    Loader {
        id: customTest

        active: false //root.mediaIndicator !== null && root.mediaIndicator.active === true

        sourceComponent: MouseArea {
            id: mareainteract
            readonly property rect mediaIndicatorPos: {
                root.mediaIndicator.x;
                root.mediaIndicator.y;
                return root.mediaIndicator.mapToItem(root, 0, 0);
            }

            readonly property point mprisPopupPos: {
                root.panels.mprisViewer.x;
                root.panels.mprisViewer.y;
                return root.panels.mprisViewer.mapToItem(root, 0, 0);
            }

            readonly property rect miAreaRect: Qt.rect(x - mediaIndicatorPos.x, y, root.mediaIndicator.implicitWidth, root.bar.implicitHeight)

            hoverEnabled: true

            function isInZone(mx, my) {
                console.log(mx, my);
                return (mx => miAreaRect.x && mx <= miAreaRect.width && my >= miAreaRect.y && my <= miAreaRect.height);
            }

            readonly property bool inZone: containsMouse && isInZone(mouseX, mouseY)

            onInZoneChanged: {
                console.log(inZone);
            }

            x: Math.min(mediaIndicatorPos.x, mprisPopupPos.x) ?? 0
            y: 0

            implicitWidth: Math.max(root.mediaIndicator.implicitWidth, root.panels.mprisViewer.implicitWidth)
            implicitHeight: root.bar.implicitHeight + root.panels.mprisViewer.implicitHeight
        }
    }
}
