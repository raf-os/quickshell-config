pragma ComponentBehavior: Bound

import qs.components
import qs.config
import Quickshell
import QtQuick

/**
 * TODO:
 * * Improve performance by caching whatever we can
 * * Implement fuzzy finding, hopefully not through JS but with C++ plugin
 * * Implement ranked search, putting most used apps at the top
 */

Item {
    id: root

    required property PersistentProperties openPanels
    required property var panels

    readonly property real initialWidth: content.implicitWidth
    readonly property int animDuration: 300
    readonly property int isActive: openPanels.startmenu

    visible: width > 0
    clip: true

    implicitHeight: content.implicitHeight
    implicitWidth: 0

    onIsActiveChanged: {
        if (isActive) {
            timer.stop();
            exitAnim.stop();
            enterAnim.start();
        } else {
            enterAnim.stop();
            exitAnim.start();
        }
    }

    // HyprlandFocusGrab {
    //     id: grab
    //     windows: [QsWindow.window]
    //     // active: root.isActive && root.visible && !timer.running
    //     onCleared: {
    //         root.openPanels.startmenu = false;
    //     }
    // }

    SequentialAnimation {
        id: enterAnim

        NAnim {
            target: root
            property: "implicitWidth"
            to: root.initialWidth
            duration: root.animDuration
            easing.bezierCurve: Config.appearance.animCurves.defaultEase
        }
        ScriptAction {
            script: {
                root.implicitWidth = Qt.binding(() => content.implicitWidth);
                content?.item?.forceFocusInput();
            }
        }
    }

    SequentialAnimation {
        id: exitAnim

        ScriptAction {
            script: {
                root.implicitWidth = root.implicitWidth;
            }
        }
        NAnim {
            target: root
            property: "implicitWidth"
            to: 0
            easing.bezierCurve: Config.appearance.animCurves.defaultEase
        }
    }

    Timer {
        id: timer

        interval: root.animDuration

        onRunningChanged: {
            if (running && !root.isActive) {
                // content.visible = false;
                content.active = false;
            } else {
                content.active = Qt.binding(() => root.isActive || root.visible);
                content.visible = true;
                if (enterAnim.running) {
                    enterAnim.stop();
                    enterAnim.start();
                }
            }
        }
    }

    Loader {
        id: content

        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        visible: false
        active: false
        Component.onCompleted: timer.start()

        onActiveChanged: {
            if (!item)
                return;
        }

        sourceComponent: Content {
            openPanels: root.openPanels
        }
    }
}
