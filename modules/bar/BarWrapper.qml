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
    required property Item panels

    readonly property int contentHeight: Config.bar.sizes.innerHeight + Config.border.thickness * 2
    readonly property int exclusiveZone: contentHeight

    readonly property Item mediaInfo: content.item ? content.item?.mediaInfo : null
    implicitHeight: contentHeight

    function handleMouseWheel(x: real, angleDelta: point) {
        content.item?.handleMouseWheel(x, angleDelta);
    }

    Loader {
        id: content

        anchors.centerIn: parent

        sourceComponent: Bar {
            anchors.centerIn: parent

            width: root.width - Config.appearance.padding.lg
            height: Config.bar.sizes.innerHeight
            screen: root.screen

            openPanels: root.openPanels
            popouts: root.popouts
            panels: root.panels
        }
    }
}
