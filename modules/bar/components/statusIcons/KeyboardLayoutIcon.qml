pragma ComponentBehavior: Bound

import qs.config
import qs.services
import qs.components
import Quickshell
import QtQuick

Item {
    id: root

    property string currentLayout: Hypr.currentLayout.slice(0, 2)

    implicitWidth: Config.appearance.fontSize.xl
    // implicitHeight: Config.bar.sizes.innerHeight

    function setCurrentLayout(code: string): void {
        currentLayout = code.slice(0, 2);
    }

    StyledText {
        id: displayText
        anchors.fill: parent

        text: root.currentLayout
        font.family: Config.appearance.fontFamily.sans
        font.weight: 600
        font.pointSize: Config.appearance.fontSize.xxs

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter

        opacity: switchCooldown.running ? 0.5 : 1
    }

    Timer {
        id: switchCooldown

        interval: 500
    }

    MouseArea {
        id: interactionArea

        anchors.fill: parent

        cursorShape: Qt.PointingHandCursor
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onClicked: event => {
            if (event.button === Qt.LeftButton) {
                if (switchCooldown.running)
                    return;

                Quickshell.execDetached(["hyprctl", "switchxkblayout", "current", "next"]);
                switchCooldown.running = true;
            }
        }
    }
}
