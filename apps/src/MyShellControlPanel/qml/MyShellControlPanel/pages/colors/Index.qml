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
    readonly property list<string> themeList: Colors.themeList
    property bool isChanged: false

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

            SCombobox {
                model: root.themeList
            }
        }

        Item {
            Layout.fillHeight: true
        }

        RowLayout {
            id: buttonsLayout

            Layout.fillWidth: true

            SButton {
                id: saveButton
                text: "Save"
                autoWidth: true
                Layout.fillWidth: true
                disabled: !root.isChanged
            }

            SButton {
                id: cancelButton
                text: "Cancel"
                autoWidth: true
                Layout.fillWidth: true
                type: SButton.ButtonType.Destructive
                disabled: !root.isChanged
            }
        }
    }
}
