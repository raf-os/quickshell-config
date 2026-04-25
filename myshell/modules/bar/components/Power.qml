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

    readonly property bool isHover: mouseArea.containsMouse
    readonly property bool isActive: isHover || root.openPanels.session

    // implicitWidth: Config.appearance.fontSize.xl
    Layout.fillHeight: true
    implicitWidth: height

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

    Rectangle {
        anchors.centerIn: parent

        implicitWidth: root.height
        implicitHeight: implicitWidth

        radius: implicitWidth
        opacity: root.isActive ? 0.25 : 0
        scale: root.isActive ? 1 : 0.5
        color: ColorService.current.destructive

        Behavior on opacity {
            NAnim {
                duration: 300
            }
        }

        Behavior on scale {
            NAnim {
                duration: 500
                easing.bezierCurve: Config.appearance.animCurves.accelerateOverCorrect
            }
        }
    }

    MaterialIcon {
        id: icon

        anchors.fill: parent

        text: "power_settings_new"
        color: root.isActive ? ColorService.current.destructiveHover : ColorService.current.destructive
        font.bold: true
        font.pointSize: Config.appearance.fontSize.md

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
