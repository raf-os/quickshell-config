pragma ComponentBehavior: Bound

import org.nightshell.Components
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

            SVGIcon {
                id: titleIcon
                iconName: "settings"
                color: Colors.colors.baseContent

                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter

                size: 24
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

            spacing: Config.appearance.spacing.sm

            anchors.top: titleLabel.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            anchors.margins: Config.appearance.padding.sm

            clip: true

            activeFocusOnTab: true

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
                implicitHeight: Math.max(itemIcon.height, itemTxt.height)

                function activate() {
                    root.app.stackInterface.clearAndPush(listItem.path);
                }

                SVGIcon {
                    id: itemIcon
                    iconName: listItem.icon

                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter

                    anchors.leftMargin: listItem.padding

                    size: 20
                    color: Colors.colors.baseContent
                }

                StyledText {
                    id: itemTxt
                    text: listItem.label

                    font.pointSize: Config.appearance.fontSize.sm
                    font.weight: listItem.isActive ? 700 : 500

                    anchors.left: itemIcon.right
                    anchors.leftMargin: Config.appearance.spacing.sm
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
