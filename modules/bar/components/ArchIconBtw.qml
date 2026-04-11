pragma ComponentBehavior: Bound

import qs.services
import qs.config
import qs.components
import qs.utils
import QtQuick
import QtQuick.Layouts
import QtQuick.Effects
import Quickshell
import Quickshell.Widgets

Item {
    id: root

    required property PersistentProperties openPanels
    required property Item panels
    readonly property bool isMenuActive: openPanels.startmenu

    // implicitWidth: Config.appearance.fontSize.xl
    Layout.fillHeight: true
    implicitWidth: height

    MouseArea {
        cursorShape: Qt.PointingHandCursor
        // propagateComposedEvents: true
        anchors.centerIn: parent

        implicitWidth: parent.width
        implicitHeight: parent.height

        onClicked: ev => {
            // root.openPanels.startmenu = !root.openPanels.startmenu;
            root.panels.openExclusivePanel("startmenu");
            ev.accepted = false;
        }
    }

    IconImage {
        id: osImage

        anchors.fill: parent

        property real brightness: root.isMenuActive ? 0.9 : 0.5

        source: SysInfo.osLogo
        scale: root.isMenuActive ? 1 : 0.8
        // implicitSize: parent.height

        Behavior on scale {
            NAnim {
                duration: 300
            }
        }

        Behavior on brightness {
            NAnim {
                duration: 300
            }
        }

        layer.enabled: true
        layer.effect: MultiEffect {
            brightness: osImage.brightness
            colorization: 1
            colorizationColor: ColorService.current.baseContent
            shadowEnabled: !GlobalStateManager.isGameMode
            shadowColor: ColorService.current.baseContent
            shadowOpacity: root.isMenuActive ? 1 : 0
            shadowScale: 1
            blurMax: 32
        }
    }
}
