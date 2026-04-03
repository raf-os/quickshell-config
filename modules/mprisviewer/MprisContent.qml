pragma ComponentBehavior: Bound

import "mpris"
import qs.services
import qs.config
import Quickshell
import Quickshell.Services.Mpris
import QtQuick
import QtQuick.Layouts

Item {
    id: root

    required property PersistentProperties openPanels
    readonly property MprisPlayer currentActive: MprisService.currentActive
    readonly property list<MprisPlayer> playerList: MprisService.playerList.filter(mp => mp !== currentActive)
    readonly property int navButtonSize: Config.appearance.fontSize.xl * 2

    implicitWidth: 360
    implicitHeight: layoutRoot.implicitHeight

    Item {
        id: previousButtonWrapper

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        implicitWidth: root.navButtonSize

        TextIcon {
            text: ""
        }
    }

    StackLayout {
        id: layoutRoot

        currentIndex: 0

        anchors.left: previousButtonWrapper.right
        anchors.right: nextButtonWrapper.left
        anchors.bottom: parent.bottom

        clip: true

        Loader {
            active: root.currentActive !== null

            sourceComponent: MprisPlayerItem {
                modelData: root.currentActive
            }
        }
    }

    Item {
        id: nextButtonWrapper

        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        implicitWidth: root.navButtonSize

        TextIcon {
            text: ""
        }
    }

    component TextIcon: Text {
        anchors.fill: parent

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        color: ColorService.current.base3

        font.family: Config.appearance.fontFamily.monoIcon
        font.pixelSize: parent.implicitWidth
    }
}
