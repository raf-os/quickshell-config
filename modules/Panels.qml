pragma ComponentBehavior: Bound

import qs.config
import qs.modules.session as Session
import qs.modules.mprisviewer as MprisViewer
import qs.modules.bar.popouts as BarPopouts
import qs.modules.startmenu as StartMenuPopout
import qs.modules.commandcapture
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
    readonly property alias mprisViewer: mprisViewer

    anchors.fill: parent
    anchors.margins: Config.border.thickness
    anchors.topMargin: bar.implicitHeight

    CommandCaptureWrapper {
        id: commandcapture
        openPanels: root.openPanels
        panels: root

        anchors.right: parent.right
        anchors.bottom: parent.bottom
        // anchors.verticalCenter: parent.verticalCenter
        // anchors.top: parent.top
        // anchors.bottom: parent.bottom
    }

    MprisViewer.MprisWrapper {
        id: mprisViewer

        anchors.top: parent.top

        x: root.bar.mediaInfo ? ((root.bar.mediaInfo.x + root.bar.mediaInfo.width / 2) - (implicitWidth / 2)) : 0

        Component.onCompleted: {
            console.log(root.bar.mediaInfo);
        }
        openPanels: root.openPanels
    }

    StartMenuPopout.Wrapper {
        id: startmenu
        openPanels: root.openPanels
        panels: root
        screen: root.screen

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
