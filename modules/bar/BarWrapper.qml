pragma ComponentBehavior: Bound

import qs.components
import "popouts"
import qs.config
import Quickshell
import QtQuick

Item {
    id: root

    required property ShellScreen screen
    required property PersistentProperties openPanels
    required property PopoutWrapper popouts

    readonly property int contentHeight: Config.bar.sizes.innerHeight + Config.border.thickness * 2
    readonly property int exclusiveZone: contentHeight
    implicitHeight: contentHeight

    Loader {
        id: content

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        sourceComponent: Bar {
            height: root.contentHeight
            screen: root.screen

            openPanels: root.openPanels
            popouts: root.popouts
        }
    }
}
