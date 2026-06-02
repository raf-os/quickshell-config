pragma ComponentBehavior: Bound

import MyShellPlugin
import MyShellPlugin.Configs
import MyShellControlPanel.components
import MyShellControlPanel.components.forms
import MyShellControlPanel.plugin
import QtQuick
import QtQuick.Effects
import QtQuick.Layouts
import QtQuick.Controls

PageStackItem {
    id: root
    title: "Colors"

    readonly property ColorConfigMetadata themeData: Colors.isPreviewing ? Colors.metadataPreview : Colors.metadata
    readonly property list<string> themeList: Colors.themeList
    readonly property string selectedTheme: Colors.themeName

    property bool isChanged: false
    property bool themeNameDirty: Colors.isPreviewing

    function debounceThemeName(name: string): void {
        debounceBuffer.desiredName = name;
        themeNameDebouncer.restart();
    }

    Component.onCompleted: {
        debounceBuffer.desiredName = Colors.themeName;
    }

    QtObject {
        id: debounceBuffer

        property string desiredName: "default"
    }

    Timer {
        id: themeNameDebouncer
        interval: 750
        running: false

        onTriggered: {
            Colors.loadPreview(debounceBuffer.desiredName, root);
            formController.resetForm();
        }
    }

    FormController {
        id: formController
        models: Colors.isPreviewing ? [Colors.colorsPreview] : [Colors.colors] // qmllint disable missing-type
    }

    ColumnLayout {
        id: mainLayout

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

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

                text: `**Theme**: ${root.themeData?.name ?? ""}\n\n**Author:** ${root.themeData?.author ?? ""}\n\n**Version:** ${root.themeData?.version ?? ""}`
                textFormat: Text.MarkdownText
                padding: Config.appearance.padding.md
            }
        }

        RowLayout {
            id: themeSelector
            Layout.fillWidth: true
            spacing: Config.appearance.spacing.md

            SCombobox {
                id: themeNameControl

                model: root.themeList
                Layout.fillWidth: true
                enabled: !themeNameDebouncer.running

                Component.onCompleted: {
                    currentIndex = find(root.selectedTheme);
                }

                onActivated: {
                    root.debounceThemeName(currentValue);
                }
            }

            SButton {
                text: "Save"

                disabled: !root.themeNameDirty || themeNameDebouncer.running

                Layout.fillHeight: true
                implicitWidth: 128

                onClicked: {
                    Colors.themeName = Colors.previewThemeName;
                    Colors.closePreview();
                }
            }
        }

        GridLayout {
            columns: 2

            uniformCellWidths: true

            Layout.fillWidth: true

            Repeater {
                model: formController.fields

                delegate: Loader {
                    id: ld
                    required property FieldController modelData
                    active: !!modelData
                    Layout.fillWidth: true

                    sourceComponent: ColorSelector {
                        controller: ld.modelData

                        name: ld.modelData?.name ?? ""
                    }
                }
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

                onClicked: {
                    Colors.saveConfig();
                }
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
