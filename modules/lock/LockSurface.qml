import Quickshell.Wayland
import QtQuick

WlSessionLockSurface {
    id: root

    required property WlSessionLock lock
    required property Pam pam

    Item {
        id: lockContent

        anchors.fill: parent

        Rectangle {
            id: lockBackgroundSolid
            anchors.fill: parent

            color: "black"
        }

        Content {
            id: content

            lock: root
        }
    }
}
