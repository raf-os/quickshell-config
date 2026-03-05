pragma ComponentBehavior: Bound

import qs.components
import qs.services
import qs.config
import Quickshell
import QtQuick
import QtQuick.Layouts

ColumnLayout {
    id: root

    required property PersistentProperties openPanels
    readonly property int rounding: Config.appearance.rounding.md
    readonly property real padding: Config.appearance.padding.lg
    readonly property bool isActive: openPanels.startmenu

    focus: isActive

    spacing: -padding

    Keys.onEscapePressed: {
        openPanels.startmenu = false;
    }
    function triggerFocus(shouldFocus: bool) {
        if (shouldFocus)
            cmdinputtxt.forceActiveFocus();
    }

    StyledText {
        text: "Start"
        font.pointSize: Config.appearance.fontSize.lg
        font.weight: 700

        Layout.margins: root.padding
    }

    Item {
        id: cmdinput

        Layout.fillWidth: true
        Layout.margins: root.padding

        implicitHeight: cmdinputtxt.implicitHeight

        Rectangle {
            id: cmdinputbg
            anchors.fill: parent
            radius: Config.appearance.rounding.sm

            color: ColorService.current.base0
        }

        TextInput {
            id: cmdinputtxt

            focus: root.isActive

            font.pointSize: Config.appearance.fontSize.md
            font.family: Config.appearance.fontFamily.mono

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            padding: 8

            color: ColorService.current.baseContent
        }
    }

    CommandSelection {}

    Item {
        implicitWidth: 320
        Layout.fillHeight: true
    }

    component CommandSelection: Item {
        id: commandSelection

        Layout.fillWidth: true
        Layout.margins: root.padding

        implicitHeight: cmdRect.implicitHeight

        StyledRect {
            id: cmdRect

            anchors.left: parent.left
            anchors.right: parent.right

            implicitHeight: 64
            radius: root.rounding

            color: ColorService.current.base2
        }

        RowLayout {
            anchors.fill: parent
            anchors.margins: root.padding
            spacing: 8

            Column {
                Layout.fillWidth: false
                Layout.minimumWidth: 48
                StyledText {
                    text: ""
                    font.pixelSize: 32
                }
            }

            Column {
                id: cmdTitleDesc
                Layout.fillWidth: true
                spacing: 4
                StyledText {
                    text: "Title T"
                    font.weight: 600
                    font.pointSize: Config.appearance.fontSize.sm
                }

                StyledText {
                    text: "Content"

                    font.pointSize: Config.appearance.fontSize.sm
                    font.italic: true
                    color: ColorService.current.baseContentMuted
                }
            }
        }
    }
}
