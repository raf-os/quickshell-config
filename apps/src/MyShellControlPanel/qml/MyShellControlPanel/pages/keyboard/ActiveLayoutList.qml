pragma ComponentBehavior: Bound

import MyShellPlugin
import MyShellPlugin.Configs
import MyShellControlPanel.plugin
import MyShellControlPanel.components
import MyShellControlPanel.singletons
import QtQuick
import QtQuick.Layouts

Item {
    id: root

    Layout.fillWidth: true
    Layout.margins: Config.appearance.padding.sm
    implicitHeight: 128

    required property KeyboardSettingsBuffer settingsBuffer
    readonly property int selectedIndex: settingsBuffer.selectedId

    function setSelectedIndex(idx: int): void {
        settingsBuffer.selectedId = idx;
    }

    Rectangle {
        id: background

        anchors.fill: parent
        color: Colors.colors.base

        radius: Config.appearance.rounding.sm
    }

    SListView {
        id: layoutSelectList
        model: root.settingsBuffer.layouts

        readonly property bool isFocused: listView.activeFocus

        anchors.fill: parent
        anchors.margins: Config.appearance.padding.sm
        clip: true

        listView.activeFocusOnTab: true

        spacing: Config.appearance.spacing.xxs

        highlightFollowsCurrentItem: false
        hightlight: Rectangle {
            id: listHighlight

            implicitWidth: ListView.view ? ListView.view.width : 0
            implicitHeight: ListView.view && ListView.view.currentItem ? ListView.view.currentItem.height : 0

            visible: (ListView.view && ListView.view.currentItem && root.enabled) ? true : false

            y: ListView.view && ListView.view.currentItem ? ListView.view.currentItem.y : 0

            color: "transparent"
            radius: Config.appearance.rounding.sm

            border.width: 2
            border.color: (ListView.view && ListView.view.activeFocus) ? Colors.colors.primary : "transparent"

            Behavior on y {
                NAnim {
                    duration: 150
                }
            }
        }

        delegate: Item {
            id: kbLayout

            required property HyprKeyboardLayout modelData
            required property int index
            readonly property bool isActive: root.selectedIndex === index
            readonly property bool isRemoveIconActive: root.settingsBuffer.layouts.length > 1

            implicitWidth: ListView.view ? ListView.view.width : 0
            implicitHeight: layoutName.height

            Rectangle {
                id: selBg

                anchors.fill: parent
                radius: Config.appearance.rounding.sm

                color: Colors.colors.base4
                opacity: kbLayout.isActive ? (root.enabled ? 1 : 0.5) : 0

                Behavior on opacity {
                    NAnim {
                        duration: 300
                    }
                }
            }

            MouseArea {
                anchors.left: parent.left
                anchors.right: itemRemoveIcon.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom

                cursorShape: root.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                hoverEnabled: root.enabled

                onClicked: {
                    if (!root.enabled)
                        return;
                    root.setSelectedIndex(kbLayout.index);
                }

                StyledText {
                    id: layoutName
                    text: kbLayout.modelData?.description ?? ""

                    font.pointSize: Config.appearance.fontSize.sm
                    font.weight: kbLayout.isActive ? 600 : 500

                    leftPadding: Config.appearance.padding.sm
                    rightPadding: leftPadding

                    topPadding: Config.appearance.padding.xs
                    bottomPadding: topPadding

                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            MouseArea {
                id: itemRemoveIcon

                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.right: parent.right

                implicitWidth: minusIcon.font.pixelSize + Config.appearance.padding.sm * 2

                enabled: kbLayout.isRemoveIconActive
                cursorShape: kbLayout.isRemoveIconActive ? Qt.PointingHandCursor : Qt.ArrowCursor
                hoverEnabled: true

                onClicked: {
                    root.settingsBuffer.removeLayoutAtIndex(kbLayout.index);
                }

                SMaterialIcon {
                    id: minusIcon

                    text: "remove"

                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter

                    font.pixelSize: Config.appearance.fontSize.lg
                    font.weight: 700
                    color: itemRemoveIcon.containsMouse ? Colors.colors.primary : Colors.colors.baseContent
                    opacity: kbLayout.isRemoveIconActive ? 1 : 0.5

                    anchors.centerIn: parent
                }
            }
        }
    }
}
