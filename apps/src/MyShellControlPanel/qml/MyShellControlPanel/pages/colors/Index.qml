pragma ComponentBehavior: Bound

import MyShellPlugin
import MyShellPlugin.Configs
import MyShellControlPanel.components
import QtQuick
import QtQuick.Effects
import QtQuick.Layouts
import QtQuick.Controls

PageStackItem {
    id: root
    title: "Colors"

    readonly property ColorConfigMetadata themeData: Colors.metadata

    ColumnLayout {
        id: mainLayout

        anchors.fill: parent

        spacing: Config.appearance.spacing.md

        Item {
            id: themeMetadata
            Layout.fillWidth: true
            implicitHeight: themeMetadataText.height

            Rectangle {
                anchors.fill: parent
                color: Colors.colors.base
                radius: Config.appearance.rounding.md
            }

            StyledText {
                id: themeMetadataText

                text: `**Theme**: ${root.themeData.name}\n\n**Author:** ${root.themeData.author}\n\n**Version:** ${root.themeData.version}`
                textFormat: Text.MarkdownText
                padding: Config.appearance.padding.md
            }
        }

        LabelWrapper {
            id: themeSelector
            text: "Selected theme"
            Layout.fillWidth: true

            SCombobox {}
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
