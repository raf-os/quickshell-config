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

    readonly property int paddingH: Config.appearance.padding.xl

    readonly property Item mediaInfo: content.item ? content.item?.mediaInfo : null
    implicitHeight: contentHeight

    function handleMouseWheel(x: real, y: real, angleDelta: point) {
        content.item?.handleMouseWheel(x, y, angleDelta);
    }

    Loader {
        id: content

        anchors.fill: parent

        sourceComponent: Bar {
            anchors.fill: parent

            anchors.topMargin: Config.border.thickness
            anchors.bottomMargin: Config.border.thickness
            anchors.leftMargin: root.paddingH
            anchors.rightMargin: root.paddingH

            // implicitHeight: Config.bar.sizes.innerHeight
            screen: root.screen

            openPanels: root.openPanels
            popouts: root.popouts
            panels: root.panels
        }
    }
}
