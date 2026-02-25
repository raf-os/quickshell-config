pragma ComponentBehavior: Bound

import qs.config
import "components"
import "components/workspaces"
import "popouts"
import Quickshell
import Quickshell.Hyprland
import QtQuick
import QtQuick.Layouts

RowLayout {
    id: root

    required property ShellScreen screen
    required property PersistentProperties openPanels
    required property PopoutWrapper popouts

    property int sideSize: 560

    signal closePopout

    spacing: Config.appearance.spacing.md

    anchors.left: parent.left
    anchors.right: parent.right

    anchors.leftMargin: Config.appearance.padding.xl
    anchors.rightMargin: Config.appearance.padding.xl * 2

    function onTriggerPopout(item: Item, name: string): void {
        const currentCenter = item.mapToItem(root, item.implicitHeight / 2, 0).x;
        const newName = popouts.toggle(item, name, currentCenter);
        popoutHandler.selectedPopoutId = newName;
    }

    function handleMouseWheel(x: real, angleDelta: point) {
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

    PopoutHandler {
        id: popoutHandler
        Component.onCompleted: {
            triggerPopout.connect(root.onTriggerPopout);
        }
        Component.onDestruction: {
            triggerPopout.disconnect(root.onTriggerPopout);
        }
    }

    RowLayout {
        id: leftContent
        property string name: "leftContent"
        // Layout.preferredWidth: Math.max(leftContent.implicitWidth, rightContent.implicitWidth)
        Layout.maximumWidth: root.sideSize
        spacing: root.spacing

        ArchIconBtw {}
        Workspaces {
            screen: root.screen
        }
        Spacing {}
    }

    WindowTitle {
        property string name: "midContent"
        Layout.fillWidth: true
    }

    RowLayout {
        id: rightContent
        // Layout.preferredWidth: Math.max(leftContent.implicitWidth, rightContent.implicitWidth)
        Layout.maximumWidth: root.sideSize
        spacing: root.spacing

        Spacing {}
        StatusIcons {
            popoutHandler: popoutHandler
        }
        Tray {
            popoutHandler: popoutHandler
        }
        ClockWidget {}
        Power {
            id: powerWidgetButton
            openPanels: root.openPanels
        }
    }

    component Spacing: Item {
        Layout.fillWidth: true
    }
}
