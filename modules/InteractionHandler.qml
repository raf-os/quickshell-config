import "bar/popouts"
import qs.config
import Quickshell
import QtQuick

MouseArea {
    id: root

    required property ShellScreen screen
    required property PersistentProperties openPanels
    required property PopoutWrapper popouts

    acceptedButtons: Qt.LeftButton | Qt.RightButton
    anchors.fill: parent

    onClicked: event => {
        console.log("test");
        popouts.close();
    }
}
