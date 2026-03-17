import qs.components
import qs.config
import qs.services
import MyShellPlugin
import Quickshell
import QtQuick
import QtQuick.Layouts

Item {
    id: root

    required property CmdEntry modelData
    required property PersistentProperties openPanels

    implicitWidth: parent?.width ?? 100
    implicitHeight: 32

    RowLayout {
        id: cmdEntry

        anchors.fill: parent
        anchors.margins: Config.appearance.padding.md
        spacing: Config.appearance.spacing.xs

        StyledText {
            id: cmdIcon
            text: root.modelData.icon === "" ? "" : root.modelData.icon
            font.family: Config.appearance.fontFamily.monoIcon
            font.pointSize: 24
        }

        ColumnLayout {
            id: cmdContent

            StyledText {
                text: root.modelData.prefix
                font.family: Config.appearance.fontFamily.mono
                font.pointSize: Config.appearance.fontSize.md
            }

            StyledText {
                text: root.modelData.label
                font.family: Config.appearance.fontFamily.sans
            }
        }
    }
}
