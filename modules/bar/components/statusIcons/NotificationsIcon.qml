import qs.components
import qs.config
import Quickshell
import Quickshell.Io
import QtQuick
import QtQuick.Layouts

MouseArea {
    id: root
    property int activeNotifications: 0

    cursorShape: Qt.PointingHandCursor
    implicitWidth: icon.implicitWidth
    implicitHeight: icon.implicitHeight
    acceptedButtons: Qt.LeftButton | Qt.RightButton

    onClicked: ev => {
        Quickshell.execDetached(['swaync-client', '-t']);
    }

    StyledText {
        id: icon
        text: root.activeNotifications > 0 ? "󱅫" : "󰂜"
        font.pointSize: Config.appearance.fontSize.md
    }

    Process {
        running: true
        command: ["swaync-client", "-s"]
        stdout: SplitParser {
            onRead: data => {
                try {
                    const jData = JSON.parse(data);
                    root.activeNotifications = jData.count;
                } catch (e) {
                    console.log("Failed parsing notification json");
                }
            }
        }
    }
}
