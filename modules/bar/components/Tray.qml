pragma ComponentBehavior: Bound

import qs.components
import qs.config
import qs.services
import qs.modules.bar
import Quickshell.Services.SystemTray
import QtQuick
import QtQuick.Layouts

StyledRect {
    id: root

    required property PopoutHandler popoutHandler

    readonly property alias layout: layout
    readonly property alias items: items

    readonly property int padding: Config.appearance.padding.sm
    readonly property int spacing: Config.appearance.spacing.md

    implicitWidth: layout.implicitWidth + padding * 2
    implicitHeight: Config.bar.sizes.innerHeight

    clip: true
    visible: width > 0

    color: ColorService.current.base0
    radius: Config.appearance.rounding.sm

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
