import "bar"
import "bar/popouts"
import qs.config
import Quickshell
import QtQuick

MouseArea {
    id: root

    required property ShellScreen screen
    required property PersistentProperties openPanels
    required property PopoutWrapper popouts
    required property BarWrapper bar

    acceptedButtons: Qt.LeftButton | Qt.RightButton
    anchors.fill: parent

    onClicked: event => {
        if (event.y < Config.border.thickness * 2 + Config.bar.sizes.innerHeight) {
            popouts.close();
        }
    }

    onWheel: event => {
        bar.handleMouseWheel(event.x, event.angleDelta);
    }
}
