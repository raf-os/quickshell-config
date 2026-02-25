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
    Layout.leftMargin: Config.appearance.spacing.md

    required property PersistentProperties openPanels

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

        anchors.fill: undefined
        anchors.centerIn: parent
        implicitWidth: implicitHeight
        implicitHeight: icon.implicitHeight + Config.appearance.padding.xs * 2

        onClicked: event => onClicked(event)

        function onClicked(): void {
            root.openPanels.session = !root.openPanels.session;
            root.powerButtonActivate();
        }
    }

    MaterialIcon {
        id: icon

        anchors.centerIn: parent

        text: "power_settings_new"
        color: mouseArea.containsMouse ? ColorService.current.destructiveHover : ColorService.current.destructive
        font.bold: true
        font.pointSize: Config.appearance.fontSize.md

        Behavior on color {
            CAnim {}
        }
    }
}
