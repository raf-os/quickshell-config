pragma ComponentBehavior: Bound

import MyShellControlPanel.components
import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick

Item {
    id: root

    required property App app

    implicitWidth: 320

    Rectangle {
        anchors.fill: parent
        color: Colors.colors.base
    }

    Item {
        id: wrapper
        anchors.fill: parent
        anchors.margins: Config.appearance.padding.lg

        Item {
            id: titleLabel
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            implicitHeight: 32

            StyledText {
                id: titleIcon
                anchors.verticalCenter: parent.verticalCenter
                text: ""

                font.family: Config.appearance.fontFamily.mono
                font.pointSize: Config.appearance.fontSize.lg
            }

            StyledText {
                anchors.left: titleIcon.right
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: Config.appearance.spacing.sm
                text: "Control panel"

                font.pointSize: Config.appearance.fontSize.lg
                font.weight: 600
            }
        }

        Rectangle {
            id: focusNotify
            anchors.fill: menuItems

            color: "transparent"
            border.width: 2

            opacity: menuItems.activeFocus ? 1 : 0
        }

        ListView {
            id: menuItems

            spacing: Config.appearance.spacing.xxs

            anchors.top: titleLabel.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            anchors.margins: Config.appearance.padding.sm

            clip: true
            focus: true

            model: SidebarModel {}

            highlight: Rectangle {
                implicitWidth: menuItems.width
                implicitHeight: menuItems.currentItem.implicitHeight
                y: menuItems.currentItem.y

                color: Colors.colors.base4
                radius: Config.appearance.rounding.sm
            }
            highlightFollowsCurrentItem: false
            keyNavigationEnabled: true
            keyNavigationWraps: true

            delegate: Item {
                id: listItem

                required property string label
                required property string path
                required property int index

                required property string icon

                readonly property int padding: Config.appearance.padding.sm
                readonly property bool isSelected: menuItems.currentIndex === index
                readonly property bool isActive: root.app.stackInterface.currentPath === path

                clip: true

                anchors.left: parent.left
                anchors.right: parent.right
                implicitHeight: Config.appearance.fontSize.lg + padding * 2

                StyledText {
                    id: itemIcon
                    text: listItem.icon

                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter

                    anchors.leftMargin: listItem.padding

                    font.family: Config.appearance.fontFamily.mono
                    font.pixelSize: Config.appearance.fontSize.lg

                    width: listItem.icon === "" ? 0 : Config.appearance.fontSize.lg + Config.appearance.padding.sm
                }

                StyledText {
                    text: listItem.label

                    font.pointSize: Config.appearance.fontSize.sm
                    font.weight: listItem.isActive ? 700 : 500

                    anchors.left: itemIcon.right
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter

                    color: interactionArea.containsMouse && !listItem.isSelected ? Colors.colors.primary : Colors.colors.baseContent
                }

                MouseArea {
                    id: interactionArea

                    anchors.fill: parent

                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: {
                        root.app.stackInterface.clearAndPush(listItem.path);
                    }

                    onEntered: {
                        menuItems.currentIndex = listItem.index;
                    }
                }
            }
        }
    }
}
