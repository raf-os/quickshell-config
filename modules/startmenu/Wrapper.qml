pragma ComponentBehavior: Bound

import qs.components
import qs.config
import Quickshell
import Quickshell.Hyprland
import QtQuick

Item {
    id: root

    required property PersistentProperties openPanels
    required property var panels

    readonly property real initialWidth: content.implicitWidth

    visible: width > 0
    clip: true

    implicitHeight: content.implicitHeight
    implicitWidth: 0

    HyprlandFocusGrab {
        id: grab
        windows: [QsWindow.window]
        active: root.openPanels.startmenu
        onCleared: {
            root.openPanels.startmenu = false;
        }
    }

    states: State {
        name: "visible"
        when: root.openPanels.startmenu

        PropertyChanges {
            root.implicitWidth: root.initialWidth
        }
    }

    transitions: [
        Transition {
            from: ""
            to: "visible"

            NAnim {
                target: root
                property: "implicitWidth"
            }
        },
        Transition {
            from: "visible"
            to: ""

            NAnim {
                target: root
                property: "implicitWidth"
            }
        }
    ]

    Loader {
        id: content

        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        Component.onCompleted: {
            active = Qt.binding(() => (root.openPanels.startmenu || root.visible));
        }

        onActiveChanged: {
            if (!item)
                return;
            item?.triggerFocus?.(active);
        }

        sourceComponent: Content {
            openPanels: root.openPanels
        }
    }
}
