pragma ComponentBehavior: Bound

import qs.components
import qs.services
import qs.config
import Quickshell
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    required property PersistentProperties openPanels

    signal mouseEntered
    signal mouseExited
    signal dismissRequested

    implicitWidth: 500
    implicitHeight: popupContent.implicitHeight

    StyledRect {
        anchors.fill: parent

        color: ColorService.current.base
        // opacity: 0.9
        radius: Config.appearance.rounding.sm
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onEntered: root.mouseEntered()
        onExited: root.mouseExited()

        preventStealing: true

        ColumnLayout {
            id: popupContent

            anchors.left: parent.left
            anchors.right: parent.right
            implicitWidth: parent.width

            RowLayout {
                id: header
                Layout.fillWidth: true
                Layout.margins: Config.appearance.padding.md
                Layout.bottomMargin: 0

                StyledText {
                    id: headerTitle
                    Layout.fillWidth: true

                    text: "User script"
                    elide: Text.ElideRight
                    font.family: Config.appearance.fontFamily.sans
                    font.weight: 600
                    font.pointSize: Config.appearance.fontSize.md
                }

                MouseArea {
                    implicitWidth: closeButton.implicitWidth
                    implicitHeight: closeButton.implicitHeight

                    cursorShape: Qt.PointingHandCursor

                    onClicked: root.dismissRequested()

                    MaterialIcon {
                        id: closeButton
                        Layout.fillWidth: false
                        text: "close"

                        font.pointSize: Config.appearance.fontSize.xl
                    }
                }
            }

            StyledRect {
                id: logWrapper
                Layout.fillWidth: true
                Layout.fillHeight: true

                Layout.margins: Config.appearance.padding.sm
                Layout.topMargin: 0

                radius: Config.appearance.rounding.sm
                color: ColorService.current.base0

                implicitHeight: Math.min(cmdOutputTxt.implicitHeight, 500)

                clip: true

                ScrollView {
                    id: scrollableOutputArea
                    anchors.fill: parent

                    ScrollBar.vertical: ScrollBar {
                        policy: ScrollBar.AsNeeded
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                    }

                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                    StyledText {
                        id: cmdOutputTxt

                        text: UserCommandService.commandOutput.join("")
                        elide: Text.ElideRight

                        width: logWrapper.width
                        padding: Config.appearance.padding.sm
                        wrapMode: Text.Wrap

                        font.family: Config.appearance.fontFamily.mono
                        font.pointSize: Config.appearance.fontSize.sm
                    }
                }
            }
        }
    }
}
