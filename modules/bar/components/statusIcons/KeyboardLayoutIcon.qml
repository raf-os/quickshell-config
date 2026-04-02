pragma ComponentBehavior: Bound

import qs.config
import qs.services
import qs.components
import Quickshell.Io
import QtQuick

Item {
    id: root

    property string currentLayout: "-"

    implicitWidth: Config.appearance.fontSize.xl
    // implicitHeight: Config.bar.sizes.innerHeight

    Component.onCompleted: {
        fetchCurrentLayoutProc.running = true;
    }

    function setCurrentLayout(code: string): void {
        currentLayout = code.slice(0, 1);
    }

    StyledText {
        id: displayText
        anchors.fill: parent

        text: "us"
        font.family: Config.appearance.fontFamily.sans
        font.weight: 600

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    MouseArea {
        id: interactionArea

        anchors.fill: parent

        cursorShape: Qt.PointingHandCursor
    }

    Process {
        id: fetchCurrentLayoutProc

        command: ["sh", "-c", "localectl status | grep Keymap | awk '{print $3}'"]
        stdout: StdioCollector {
            onStreamFinished: {
                root.setCurrentLayout(this.text);
            }
        }
    }
}
