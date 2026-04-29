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
        color: Colors.colors.base2
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

            anchors.top: titleLabel.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            anchors.margins: Config.appearance.padding.sm

            clip: true

            model: SidebarModel {}

            highlight: Rectangle {
                implicitWidth: menuItems.width
                implicitHeight: menuItems.currentItem.implicitHeight
                y: menuItems.currentItem.y

                color: Colors.colors.base4
                radius: Config.appearance.rounding.sm
            }
            highlightFollowsCurrentItem: false

            delegate: Item {
                id: listItem

                required property string label
                required property string path
                required property int index

                anchors.left: parent.left
                anchors.right: parent.right
                implicitHeight: 24

                Text {
                    text: listItem.label
                    color: Colors.colors.baseContent

                    width: parent.implicitWidth
                    font.pointSize: Config.appearance.fontSize.sm

                    anchors.verticalCenter: parent.verticalCenter
                    padding: Config.appearance.padding.xs
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
