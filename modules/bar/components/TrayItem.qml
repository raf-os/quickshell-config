pragma ComponentBehavior: Bound

import qs.utils
import qs.components
import qs.services
import qs.modules.bar
import qs.modules.bar.popouts
import qs.config
import Quickshell
import Quickshell.Widgets
import Quickshell.Services.SystemTray
import QtQuick
import QtQuick.Effects

MouseArea {
    id: root

    required property SystemTrayItem modelData
    required property PopoutHandler popoutHandler
    required property int index

    readonly property string name: `traymenu${index}`
    readonly property bool isActive: popoutHandler.selectedPopoutId === name

    acceptedButtons: Qt.LeftButton | Qt.RightButton
    cursorShape: Qt.PointingHandCursor
    implicitWidth: Config.appearance.fontSize.xl
    implicitHeight: Config.appearance.fontSize.xl

    onClicked: event => {
        if (event.button === Qt.LeftButton) {
            // modelData.activate();
            popoutHandler.triggerPopout(root, root.name);
        } else {
            modelData.activate();
        }
    }

    // Rectangle {
    //     anchors.fill: parent
    //     color: root.popoutHandler.selectedPopoutId === root.name ? ColorService.current.base3 : "transparent"
    // }

    IconImage {
        id: icon

        anchors.fill: parent
        anchors.margins: root.isActive ? -2 : 0
        source: Icons.getTrayIcon(root.modelData.id, root.modelData.icon)

        asynchronous: true

        layer.enabled: true
        layer.effect: MultiEffect {
            saturation: root.isActive ? 0 : -1
        }
    }
}
