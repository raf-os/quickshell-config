pragma ComponentBehavior: Bound

import "mpris"
import qs.services
import Quickshell.Services.Mpris
import QtQuick
import QtQuick.Layouts

Item {
    id: root

    readonly property MprisPlayer currentActive: MprisService.currentActive
    readonly property list<MprisPlayer> playerList: MprisService.playerList.filter(mp => mp !== currentActive)

    implicitWidth: 320
    implicitHeight: layoutRoot.implicitHeight

    StackLayout {
        id: layoutRoot

        currentIndex: 0

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        MprisPlayerItem {
            modelData: root.currentActive
        }
    }
}
