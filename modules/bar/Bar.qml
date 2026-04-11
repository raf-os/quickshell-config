pragma ComponentBehavior: Bound

import qs.config
import "components"
import "components/workspaces"
import "popouts"
import Quickshell
import Quickshell.Hyprland
import QtQuick
import QtQuick.Layouts

Item {
    id: root

    required property ShellScreen screen
    required property PersistentProperties openPanels
    required property PopoutWrapper popouts
    required property Item panels

    property alias mediaInfo: mediaInfo

    property int middleSize: 560

    signal closePopout

    property int spacing: Config.appearance.spacing.md

    function onTriggerPopout(item: Item, name: string): void {
        const currentCenter = item.mapToItem(root, item.implicitWidth / 2, item.implicitHeight / 2).x + (item.width ?? 0) / 2;
        const newName = popouts.toggle(item, name, currentCenter);
        popoutHandler.selectedPopoutId = newName;
    }

    function handleMouseWheel(x: real, y: real, angleDelta: point) {
        if (y > root.implicitHeight)
            return;
        const ch = childAt(x, root.height / 2);
        if (ch?.name === "leftContent" || ch?.name === "midContent") {
            const angleDir = Math.sign(angleDelta.y);
            Hyprland.dispatch(`workspace m${angleDir > 0 ? "-1" : "+1"}`);
        }
    }

    Connections {
        target: root.popouts

        function onPopoutClosed() {
            popoutHandler.selectedPopoutId = null;
        }
    }

    Connections {
        target: powerWidgetButton

        function onPowerButtonActivate() {
            root.popouts.close();
        }
    }

    Connections {
        target: popoutHandler

        function onTriggerPopout(item: Item, name: string) {
            root.onTriggerPopout(item, name);
        }

        function onClosePopout() {
            root.popouts.close();
        }
    }

    PopoutHandler {
        id: popoutHandler

        openPanels: root.openPanels
    }

    RowLayout {
        id: leftContent
        property string name: "leftContent"

        anchors.left: parent.left
        anchors.right: middleContent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        spacing: root.spacing

        ArchIconBtw {
            openPanels: root.openPanels
            panels: root.panels
        }
        Workspaces {
            screen: root.screen
        }

        Spacing {}

        MediaInfo {
            id: mediaInfo
            openPanels: root.openPanels
            panels: root.panels
        }
    }

    WindowTitle {
        id: middleContent
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        property string name: "midContent"

        implicitWidth: root.middleSize
    }

    RowLayout {
        id: rightContent

        anchors.left: middleContent.right
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        spacing: root.spacing

        Spacing {}
        StatusIcons {
            popoutHandler: popoutHandler
            popoutWrapper: root.popouts
        }
        Tray {
            popoutHandler: popoutHandler
        }
        ClockWidget {}
        Power {
            id: powerWidgetButton
            openPanels: root.openPanels
            panels: root.panels
        }
    }

    component Spacing: Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
}
