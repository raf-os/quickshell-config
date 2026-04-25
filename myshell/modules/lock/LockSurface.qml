import qs.components
import qs.services
import Quickshell.Wayland
import QtQuick

WlSessionLockSurface {
    id: root

    required property WlSessionLock lock
    required property Pam pam

    color: "transparent"

    Connections {
        target: root.lock

        function onUnlock(): void {
            lockContent.playUnlockAnim();
        }
    }

    ScreencopyView {
        id: desktopBg

        anchors.fill: parent
        captureSource: root.screen
    }

    Item {
        id: lockContent

        anchors.fill: parent

        function playUnlockAnim() {
            unlockAnim.restart();
        }

        ParallelAnimation {
            id: enterAnim
            running: true

            NAnim {
                target: lockBackgroundSolid
                property: "opacity"
                from: 0
                to: 1
                duration: 600
            }
        }

        SequentialAnimation {
            id: unlockAnim

            ParallelAnimation {
                NAnim {
                    target: lockBackgroundSolid
                    property: "opacity"
                    to: 0
                    duration: 500
                }
            }

            PropertyAction {
                target: root.lock
                property: "locked"
                value: false
            }
        }

        Rectangle {
            id: lockBackgroundSolid
            anchors.fill: parent

            color: ColorService.current.base0
            opacity: 0
        }

        Content {
            id: content

            lock: root
        }
    }
}
