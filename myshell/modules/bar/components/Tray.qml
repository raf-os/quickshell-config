pragma ComponentBehavior: Bound

import qs.components
import qs.config
import qs.services
import qs.modules.bar
import Quickshell.Services.SystemTray
import QtQuick
import QtQuick.Layouts

Item {
    id: root

    required property PopoutHandler popoutHandler

    readonly property alias layout: layout
    readonly property alias items: items

    readonly property int padding: Config.appearance.padding.sm
    readonly property int spacing: Config.appearance.spacing.md

    readonly property real initialWidth: layout.implicitWidth + padding * 2
    readonly property bool hasItems: SystemTray.items.values.length > 0

    implicitWidth: hasItems ? initialWidth : 0
    implicitHeight: Config.bar.sizes.innerHeight

    clip: true
    visible: width > 0

    Behavior on implicitWidth {
        NAnim {}
    }

    Rectangle {
        anchors.fill: parent

        color: ColorService.current.base0
        radius: Config.appearance.rounding.sm
    }

    RowLayout {
        id: layout

        anchors.centerIn: parent
        spacing: Config.appearance.spacing.sm

        Repeater {
            id: items

            model: SystemTray.items

            TrayItem {
                popoutHandler: root.popoutHandler
            }
        }
    }
}
