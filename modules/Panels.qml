pragma ComponentBehavior: Bound

import qs.config
import qs.modules.session as Session
import qs.modules.bar.popouts as BarPopouts
import qs.modules.startmenu as StartMenuPopout
import Quickshell
import Quickshell.Hyprland
import QtQuick

Item {
    id: root

    required property ShellScreen screen
    required property PersistentProperties openPanels
    required property Item bar

    readonly property alias session: session
    readonly property alias popouts: popouts
    readonly property alias startmenu: startmenu

    anchors.fill: parent
    anchors.margins: Config.border.thickness
    anchors.topMargin: bar.implicitHeight

    StartMenuPopout.Wrapper {
        id: startmenu
        openPanels: root.openPanels
        panels: root

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
    }

    Session.Wrapper {
        id: session
        openPanels: root.openPanels
        panels: root

        // anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.right: parent.right
    }

    BarPopouts.PopoutWrapper {
        id: popouts

        screen: root.screen

        x: currentCenter - baseWidth / 2
        // y: 4
    }
}
