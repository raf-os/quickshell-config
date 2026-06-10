import qs.components
import MyShellPlugin
import MyShellPlugin.Configs
import Quickshell
import Quickshell.Io
import QtQuick

MouseArea {
    id: root
    property int activeNotifications: 0

    cursorShape: Qt.PointingHandCursor
    implicitWidth: Config.appearance.fontSize.xl
    // implicitHeight: Config.bar.sizes.innerHeight
    acceptedButtons: Qt.LeftButton | Qt.RightButton

    onClicked: ev => {
        Quickshell.execDetached(['swaync-client', '-t']);
    }

    StyledText {
        id: icon
        text: root.activeNotifications > 0 ? "󰂞" : "󰂜"
        color: root.activeNotifications > 0 ? Colors.colors.baseContent : Colors.colors.baseContentMuted
        font.pixelSize: Config.appearance.fontSize.xl
        font.family: Config.appearance.fontFamily.monoIcon
        anchors.fill: parent

        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
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
