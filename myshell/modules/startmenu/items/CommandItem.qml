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
    implicitHeight: 64

    RowLayout {
        id: cmdEntry

        anchors.fill: parent
        anchors.margins: Config.appearance.padding.md
        spacing: Config.appearance.spacing.sm

        Item {
            id: cmdIconWrapper
            implicitWidth: 32
            implicitHeight: 32

            Layout.alignment: Qt.AlignVCenter

            StyledText {
                id: cmdIcon
                anchors.centerIn: parent
                text: root.modelData.icon === "" ? "" : root.modelData.icon
                font.family: Config.appearance.fontFamily.monoIcon
                font.pointSize: 24
            }
        }

        Column {
            id: cmdContent

            Layout.fillWidth: true
            spacing: 4

            StyledText {
                text: root.modelData.prefix
                font.family: Config.appearance.fontFamily.mono
                font.pointSize: Config.appearance.fontSize.sm
            }

            StyledText {
                text: root.modelData.label
                font.family: Config.appearance.fontFamily.sans
                font.pointSize: Config.appearance.fontSize.sm
                color: ColorService.current.baseContentMuted
            }
        }
    }
}
