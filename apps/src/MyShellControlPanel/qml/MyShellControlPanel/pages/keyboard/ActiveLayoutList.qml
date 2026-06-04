pragma ComponentBehavior: Bound

import MyShellPlugin
import MyShellPlugin.Configs
import MyShellControlPanel.components
import MyShellControlPanel.singletons
import QtQuick
import QtQuick.Layouts

Item {
    id: root

    Layout.fillWidth: true
    Layout.margins: Config.appearance.padding.sm
    implicitHeight: 128

    property int selectedIndex
    required property SettingsBuffer settingsBuffer

    Component.onCompleted: {
        refreshSelectedIndex();
    }

    function refreshSelectedIndex() {
        root.selectedIndex = Hypr.currentLayoutIndex;
    }

    Connections {
        target: Hypr

        function onCurrentLayoutIndexChanged() {
            root.refreshSelectedIndex();
        }
    }

    Connections {
        target: root.settingsBuffer

        function onLayoutsChanged() {
            root.refreshSelectedIndex();
        }
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
            implicitHeight: ListView.view ? ListView.view.currentItem.height : 0

            visible: (ListView.view && ListView.view.currentItem && root.enabled) ? true : false

            y: ListView.view ? ListView.view.currentItem.y : 0

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

        delegate: MouseArea {
            id: kbLayout

            required property HyprKeyboardLayout modelData
            required property int index
            readonly property bool isActive: root.selectedIndex === index

            cursorShape: root.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
            hoverEnabled: root.enabled

            implicitWidth: ListView.view ? ListView.view.width : 0
            implicitHeight: layoutName.height

            onClicked: {
                if (!root.enabled)
                    return;
                root.selectedIndex = kbLayout.index;
            }

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

            StyledText {
                id: layoutName
                text: kbLayout.modelData?.description ?? ""

                font.pointSize: Config.appearance.fontSize.sm
                font.weight: kbLayout.isActive ? 600 : 500

                leftPadding: Config.appearance.padding.sm
                rightPadding: leftPadding

                topPadding: Config.appearance.padding.xs
                bottomPadding: topPadding

                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
}
