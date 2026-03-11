import qs.components
import qs.services
import qs.config
import Quickshell
import Quickshell.Io
import QtQuick
import QtQuick.Layouts

MouseArea {
    id: root
    property int activeNotifications: 0

    cursorShape: Qt.PointingHandCursor
    implicitWidth: Config.appearance.fontSize.lg * 1.2
    implicitHeight: Config.appearance.fontSize.lg * 1.2
    acceptedButtons: Qt.LeftButton | Qt.RightButton

    onClicked: ev => {
        Quickshell.execDetached(['swaync-client', '-t']);
    }

    StyledText {
        id: icon
        text: root.activeNotifications > 0 ? "󰂞" : "󰂜"
        color: root.activeNotifications > 0 ? ColorService.current.baseContent : ColorService.current.baseContentMuted
        font.pointSize: Config.appearance.fontSize.lg
        font.family: Config.appearance.fontFamily.monoIcon
        anchors.centerIn: parent
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
