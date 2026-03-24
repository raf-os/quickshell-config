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
    implicitWidth: Config.appearance.fontSize.xl
    implicitHeight: Config.bar.sizes.innerHeight
    acceptedButtons: Qt.LeftButton | Qt.RightButton

    onClicked: ev => {
        Quickshell.execDetached(['swaync-client', '-t']);
    }

    StyledText {
        id: icon
        text: root.activeNotifications > 0 ? "󰂞" : "󰂜"
        color: root.activeNotifications > 0 ? ColorService.current.baseContent : ColorService.current.baseContentMuted
        font.pixelSize: Config.appearance.fontSize.xl
        font.family: Config.appearance.fontFamily.monoIcon
        anchors.centerIn: parent

        horizontalAlignment: Qt.AlignHCenter
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
