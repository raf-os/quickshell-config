pragma ComponentBehavior: Bound

import qs.services
import qs.config
import qs.utils
import QtQuick
import QtQuick.Effects
import Quickshell
import Quickshell.Widgets

Item {
    id: root

    required property PersistentProperties openPanels
    readonly property bool isMenuActive: openPanels.startmenu

    implicitWidth: Config.appearance.fontSize.lg * 1.2
    implicitHeight: Config.appearance.fontSize.lg * 1.2

    MouseArea {
        cursorShape: Qt.PointingHandCursor
        propagateComposedEvents: true
        anchors.fill: parent

        onClicked: ev => {
            root.openPanels.startmenu = !root.openPanels.startmenu;
            ev.accepted = false;
        }
    }

    IconImage {
        anchors.centerIn: parent
        source: SysInfo.osLogo
        implicitSize: Config.appearance.fontSize.lg * 1.2

        layer.enabled: true
        layer.effect: MultiEffect {
            brightness: 0.5
            colorization: 1
            colorizationColor: ColorService.current.baseContent
            shadowEnabled: !GlobalStateManager.isGameMode
            shadowColor: ColorService.current.baseContent
            shadowOpacity: root.isMenuActive ? 1 : 0
            shadowScale: 1.25
            blurMax: 24
        }
    }
}
