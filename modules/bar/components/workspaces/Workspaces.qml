pragma ComponentBehavior: Bound

import qs.components
import qs.config
import qs.services
import Quickshell
import Quickshell.Widgets
import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root

    required property ShellScreen screen

    readonly property int activeWsId: Hypr.monitorFor(screen).activeWorkspace?.id ?? 1
    readonly property var wsForMonitor: Hypr.workspacesForScreen(screen)
    readonly property var occupiedWorkspaces: Hypr.workspaces.values.reduce((acc, curr) => {
        acc[curr.id] = curr.lastIpcObject.windows > 0;
        return acc;
    }, {})

    implicitWidth: layout.implicitWidth + Config.appearance.padding.sm * 2
    implicitHeight: Config.bar.sizes.innerHeight

    color: ColorService.current.base0
    radius: Config.appearance.rounding.md

    Item {
        anchors.fill: parent

        RowLayout {
            id: layout

            anchors.centerIn: parent
            spacing: Config.appearance.spacing.sm

            Repeater {
                id: workspaces

                model: root.wsForMonitor

                Workspace {
                    activeWsId: root.activeWsId
                    occupied: root.occupiedWorkspaces
                }
            }
        }
    }

    Behavior on color {
        CAnim {}
    }
}
