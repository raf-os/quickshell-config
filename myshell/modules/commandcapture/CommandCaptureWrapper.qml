pragma ComponentBehavior: Bound

import qs.components
import qs.config
import qs.services
import Quickshell
import QtQuick

Item {
    id: root

    required property PersistentProperties openPanels
    required property var panels

    readonly property bool isActive: openPanels.commandCapture
    readonly property real initialWidth: content.implicitWidth + borderSpacing
    readonly property real animDuration: 300
    readonly property real borderSpacing: Config.appearance.spacing.md

    implicitWidth: 0
    implicitHeight: content.implicitHeight + borderSpacing

    visible: width > 0
    clip: true

    onIsActiveChanged: {
        if (isActive) {
            animTimer.stop();
            exitAnim.stop();
            enterAnim.start();
        } else {
            enterAnim.stop();
            exitAnim.start();
        }
    }

    Timer {
        id: animTimer

        interval: root.animDuration

        onRunningChanged: {
            if (running && !root.isActive) {
                content.visible = false;
                content.active = false;
            } else {
                content.active = Qt.binding(() => root.isActive || root.visible);
                content.visible = true;
            }
        }
    }

    Timer {
        id: dismissTimer

        interval: 3000

        onTriggered: {
            root.openPanels.commandCapture = false;
        }
    }

    SequentialAnimation {
        id: enterAnim

        NAnim {
            target: root
            property: "implicitWidth"
            to: root.initialWidth
            duration: root.animDuration
        }
        ScriptAction {
            script: {
                root.implicitWidth = Qt.binding(() => content.implicitWidth + root.borderSpacing);
                // dismissTimer.start();
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
            duration: root.animDuration
        }
    }

    Connections {
        target: content?.item
        enabled: root.isActive

        function onMouseEntered() {
            dismissTimer.stop();
        }

        function onMouseExited() {
            dismissTimer.restart();
        }

        function onDismissRequested() {
            dismissTimer.stop();
            root.openPanels.commandCapture = false;
        }
    }

    Connections {
        target: UserCommandService

        function onProcessCompleted() {
            dismissTimer.restart();
        }
    }

    Loader {
        id: content

        anchors.left: parent.left
        anchors.top: parent.top

        active: false
        visible: false
        Component.onCompleted: animTimer.start()

        sourceComponent: CommandCaptureContent {
            openPanels: root.openPanels
        }
    }
}
