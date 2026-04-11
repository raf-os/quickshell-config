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
        bar.handleMouseWheel(event.x, event.y, event.angleDelta);
    }

    Loader {
        id: customTest

        active: root.mediaIndicator !== null && root.mediaIndicator.active === true

        sourceComponent: Item {
            id: rootAreaInteract

            readonly property point mediaIndicatorPos: {
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

            x: Math.min(mediaIndicatorPos.x, mprisPopupPos.x) ?? 0
            y: 0

            property bool isMouseWithin: mediaBarArea.containsMouse || mediaPopoutArea.containsMouse

            onIsMouseWithinChanged: {
                if (isMouseWithin) {
                    root.panels.openHoverExclusivePanel("mprisViewer");
                } else {
                    root.openPanels.mprisViewer = false;
                }
            }

            MouseArea {
                id: mediaBarArea
                x: rootAreaInteract.mediaIndicatorPos.x - parent.x
                y: 0

                implicitWidth: root.mediaIndicator.width
                implicitHeight: root.bar.implicitHeight

                hoverEnabled: true

                cursorShape: Qt.PointingHandCursor

                onClicked: root.panels.openExclusivePanel("mprisViewer")
            }

            MouseArea {
                id: mediaPopoutArea
                x: rootAreaInteract.mprisPopupPos.x - parent.x
                y: rootAreaInteract.mprisPopupPos.y - parent.y

                implicitWidth: root.panels.mprisViewer.implicitWidth
                implicitHeight: root.panels.mprisViewer.implicitHeight

                hoverEnabled: true
            }
        }
    }
}
