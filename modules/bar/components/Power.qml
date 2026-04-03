import qs.components
import qs.modules.bar
import qs.services
import qs.config
import Quickshell
import Quickshell.Hyprland
import QtQuick
import QtQuick.Layouts

Item {
    id: root
    // Layout.leftMargin: Config.appearance.spacing.md

    required property PersistentProperties openPanels
    required property Item panels

    implicitWidth: Config.appearance.fontSize.xl * 2
    Layout.fillHeight: true

    signal powerButtonActivate

    HyprlandFocusGrab {
        active: root.openPanels.session
        windows: [QsWindow.window]
        onCleared: {
            root.openPanels.session = false;
        }
    }

    MouseArea {
        id: mouseArea
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor

        anchors.fill: parent

        onClicked: event => onClicked(event)

        function onClicked(): void {
            // root.openPanels.session = !root.openPanels.session;
            root.panels.openExclusivePanel("session");
            root.powerButtonActivate();
        }
    }

    MaterialIcon {
        id: icon

        anchors.fill: parent

        text: "power_settings_new"
        color: mouseArea.containsMouse ? ColorService.current.destructiveHover : ColorService.current.destructive
        font.bold: true
        font.pointSize: Config.appearance.fontSize.md

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        Behavior on color {
            CAnim {}
        }
    }
}
