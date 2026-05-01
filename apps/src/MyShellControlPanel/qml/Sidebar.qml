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

            Keys.onReturnPressed: ev => {
                currentItem?.activate(); // qmllint disable missing-property
                root.app.resetAppFocus();
            }

            model: SidebarModel {}

            highlight: Rectangle {
                implicitWidth: menuItems.width
                implicitHeight: menuItems.currentItem.implicitHeight
                y: menuItems.currentItem.y

                color: "transparent"
                radius: Config.appearance.rounding.sm

                border.width: menuItems.activeFocus ? 2 : 0
                border.color: menuItems.activeFocus ? Colors.colors.primary : "transparent"

                Behavior on border.color {
                    CAnim {
                        duration: 150
                    }
                }

                Behavior on y {
                    NAnim {
                        duration: 200
                    }
                }
            }
            highlightFollowsCurrentItem: false
            keyNavigationEnabled: true
            keyNavigationWraps: true

            delegate: Item {
                id: listItem

                required property string label
                required property string path
                required property string icon

                required property int index

                readonly property int padding: Config.appearance.padding.sm
                readonly property bool isSelected: menuItems.currentIndex === index
                readonly property bool isActive: root.app.stackInterface.currentPath === path
                readonly property bool isHovered: menuItems.activeFocus ? isSelected : interactionArea.containsMouse

                clip: true

                implicitWidth: ListView.view.width
                implicitHeight: Config.appearance.fontSize.lg + padding * 2

                function activate() {
                    root.app.stackInterface.clearAndPush(listItem.path);
                }

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

                    color: listItem.isHovered ? Colors.colors.primary : Colors.colors.baseContent
                }

                MouseArea {
                    id: interactionArea

                    anchors.fill: parent

                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: {
                        root.app.stackInterface.clearAndPush(listItem.path);
                    }
                }
            }
        }
    }
}
