pragma ComponentBehavior: Bound

import qs.components
import qs.services
import qs.config
import qs.utils
import Quickshell
import Quickshell.Widgets
import QtQuick
import QtQuick.Layouts

ColumnLayout {
    id: root
    property int rounding: Config.appearance.rounding.lg
    required property PersistentProperties openPanels

    property real padding: Config.appearance.padding.lg
    spacing: -padding

    property string hoverText: "POWER MENU"

    function updateHoverText(newText: string): void {
        if (newText === undefined || newText === "")
            return;
        root.hoverText = newText;
    }

    Rectangle {
        property real margin: Config.appearance.padding.sm
        Layout.fillWidth: true
        Layout.margins: root.padding

        color: ColorService.current.base2
        radius: Config.appearance.rounding.sm

        implicitHeight: labelText.implicitHeight + margin * 2

        StyledText {
            id: labelText
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.margins: parent.margin
            anchors.leftMargin: parent.margin * 1.5
            anchors.rightMargin: parent.margin * 1.5
            text: root.hoverText
            font.family: Config.appearance.fontFamily.mono
            font.pointSize: Config.appearance.fontSize.sm
            clip: true
        }
    }

    RowLayout {
        Layout.margins: root.padding
        spacing: Config.appearance.spacing.lg

        SessionButton {
            id: lock

            icon: "lock"
            label: "Lock"
            command: ["hyprlock"]
        }

        SessionButton {
            id: logout

            icon: "logout"
            label: "Log Out"
            command: []
        }

        SessionButton {
            id: reboot

            icon: "cached"
            label: "Reboot"
            command: []
        }

        SessionButton {
            id: shutdown

            icon: "power_settings_new"
            label: "Shut Down"
            command: ["sh", "-c", "hyprshutdown -p \"systemctl poweroff\""]
        }
    }

    component SessionButton: StyledRect {
        id: button

        required property string icon
        required property list<string> command
        property string label

        implicitWidth: 64
        implicitHeight: 64

        color: ColorService.current.base2
        radius: root.rounding

        ButtonStateLayer {
            radius: parent.radius

            color: ColorService.current.base4

            onEntered: () => root.updateHoverText(button.label)

            function onClicked(): void {
                Quickshell.execDetached(button.command);
            }
        }

        MaterialIcon {
            anchors.centerIn: parent

            text: button.icon
            font.pointSize: Config.appearance.fontSize.xl
            font.bold: true
        }
    }
}
