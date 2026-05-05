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

        StyledText {
            id: themeMetadataText

            text: `**Theme**: ${root.themeData.name}\n\n**Author:** ${root.themeData.author}\n\n**Version:** ${root.themeData.version}`
            textFormat: Text.MarkdownText
        }

        ComboBox {
            id: cbBox
            model: ["Default", "test"]

            readonly property int textPadding: Config.appearance.padding.md
            font.family: Config.appearance.fontFamily.sans
            font.pointSize: Config.appearance.fontSize.sm

            implicitWidth: 320

            delegate: ItemDelegate {
                id: delegate

                required property var model
                required property int index

                width: cbBox.width
                background: null
                contentItem: StyledText {
                    text: delegate.model[cbBox.textRole]
                    color: Colors.colors.baseContent
                    font: cbBox.font
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                }
                highlighted: cbBox.highlightedIndex === index
            }

            indicator: StyledText {
                x: cbBox.width - width

                width: cbBox.height
                height: cbBox.height
                text: ""

                font.family: Config.appearance.fontFamily.mono
                font.pointSize: Config.appearance.fontSize.md

                opacity: cbBox.popup.visible ? 1 : 0.5
                scale: cbBox.popup.visible ? 1.25 : 1

                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter

                Behavior on opacity {
                    NAnim {}
                }

                Behavior on scale {
                    NAnim {}
                }
            }

            contentItem: StyledText {
                padding: cbBox.textPadding

                text: cbBox.displayText
                font: cbBox.font
            }

            background: Rectangle {
                // implicitWidth: 320
                // implicitHeight: cbBox.font.pointSize

                color: Colors.colors.base
                border.width: cbBox.visualFocus ? 2 : 1
                border.color: cbBox.visualFocus ? Colors.colors.primary : cbBox.popup.visible ? Colors.colors.base5 : Colors.colors.base3
                radius: Config.appearance.rounding.md
            }

            popup: Popup {
                id: cbBoxPopup
                y: cbBox.height + 4
                width: cbBox.width
                height: contentItem.implicitHeight + padding * 2
                padding: 4

                readonly property int animDuration: 200

                enter: Transition {
                    ParallelAnimation {
                        NAnim {
                            property: "scale"
                            from: 0.9
                            to: 1
                            duration: cbBoxPopup.animDuration
                        }
                        NAnim {
                            property: "opacity"
                            from: 0
                            to: 1
                            duration: cbBoxPopup.animDuration
                        }
                    }
                }

                exit: Transition {
                    ParallelAnimation {
                        NAnim {
                            property: "scale"
                            from: 1
                            to: 0.9
                            duration: cbBoxPopup.animDuration
                        }
                        NAnim {
                            property: "opacity"
                            from: 1
                            to: 0
                            duration: cbBoxPopup.animDuration
                        }
                    }
                }

                contentItem: ListView {
                    id: pplv
                    clip: true
                    implicitHeight: contentHeight
                    spacing: 0
                    model: cbBox.popup.visible ? cbBox.delegateModel : null
                    currentIndex: cbBox.highlightedIndex

                    highlight: Rectangle {
                        id: highrect
                        y: ListView.view?.currentItem ? ListView.view.currentItem.y : 0
                        implicitWidth: ListView.view ? ListView.view.width : 0
                        implicitHeight: ListView.view?.currentItem ? ListView.view.currentItem.implicitHeight : 0

                        color: Colors.colors.base4
                        radius: Config.appearance.rounding.md

                        Behavior on y {
                            NAnim {
                                duration: 200
                            }
                        }
                    }
                    highlightFollowsCurrentItem: false
                }

                background: Item {
                    RectangularShadow {
                        anchors.fill: parent
                        offset: Qt.vector2d(0, 4)
                        radius: Config.appearance.rounding.md
                        color: Qt.rgba(0.0, 0.0, 0.0, 0.25)
                        blur: 8
                    }
                    Rectangle {
                        anchors.fill: parent
                        radius: Config.appearance.rounding.md
                        color: Colors.colors.base2
                        border.width: 1
                        border.color: Colors.colors.base3
                    }
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
