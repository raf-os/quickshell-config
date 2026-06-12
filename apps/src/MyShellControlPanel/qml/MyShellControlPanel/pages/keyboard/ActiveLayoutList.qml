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
    readonly property int rounding: Config.appearance.rounding.sm

    function setSelectedIndex(idx: int): void {
        settingsBuffer.selectedId = idx;
    }

    Rectangle {
        id: background

        anchors.fill: parent
        color: Colors.colors.base

        radius: Config.appearance.rounding.sm
    }

    // DropArea {
    //     id: fallbackDropArea
    //
    //     anchors.fill: parent
    //     anchors.margins: Config.appearance.padding.sm
    //
    //     keys: ["ActiveKeyboardLayout"]
    //
    //     onEntered: drag => {
    //     // This should move the item to the end of the list
    //     }
    //
    //     onDropped: drag => {
    //         console.log("dop");
    //     }
    // }

    SListView {
        id: layoutSelectList
        model: root.settingsBuffer

        readonly property bool isFocused: listView.activeFocus

        anchors.fill: parent
        anchors.margins: Config.appearance.padding.sm
        clip: true

        listView.activeFocusOnTab: true

        spacing: Config.appearance.spacing.xxs

        highlightFollowsCurrentItem: false
        highlight: Rectangle {
            id: listHighlight
            z: 2

            implicitWidth: ListView.view ? ListView.view.width : 0
            implicitHeight: ListView.view && ListView.view.currentItem ? ListView.view.currentItem.height : 0

            visible: (ListView.view && ListView.view.currentItem && root.enabled) ? true : false

            y: ListView.view && ListView.view.currentItem ? ListView.view.currentItem.y : 0

            color: "transparent"
            radius: Config.appearance.rounding.sm

            border.width: 2
            border.color: (ListView.view && ListView.view.activeFocus) ? Colors.colors.neutralContent : "transparent"

            Behavior on y {
                NAnim {
                    duration: 150
                }
            }
        }

        delegate: draggableDelegate
    }

    Component {
        id: draggableDelegate

        Item {
            id: kbLayout

            required property KeyboardLayoutItem modelData
            required property int index
            readonly property bool isActive: root.selectedIndex === index
            readonly property bool isRemoveIconActive: root.settingsBuffer.layouts.length > 1

            readonly property real desiredItemHeight: layoutName.height
            property bool dragAnchorBottom: false

            implicitWidth: ListView.view ? ListView.view.width : 0
            implicitHeight: desiredItemHeight + kbLayoutDropArea.dragItemSpacing

            DropArea {
                id: kbLayoutDropArea

                property real dragItemSpacing: 0
                property bool isDragValid: false

                anchors.fill: parent
                anchors.topMargin: -layoutSelectList.spacing
                anchors.bottomMargin: -layoutSelectList.spacing

                keys: ["ActiveKeyboardLayout"]

                onEntered: drag => {
                    if (drag.source === dragHandle)
                        return;

                    root.settingsBuffer.swapItems(drag.source.modelIndex, kbLayout.index);
                }
            }

            Rectangle {
                id: dragBgIndicator

                anchors.fill: parent

                visible: dragHandle.held

                radius: root.rounding
                color: Colors.colors.base3.alpha(0.75)
            }

            Item {
                id: kbLayoutVisualComponent

                readonly property bool isDragging: dragHandle.held

                z: isDragging ? 4 : 0
                opacity: isDragging ? 0.75 : 1

                Drag.keys: ["ActiveKeyboardLayout"]
                Drag.active: dragHandle.held
                Drag.source: dragHandle
                Drag.hotSpot.y: height / 2

                implicitWidth: kbLayout.implicitWidth
                implicitHeight: kbLayout.desiredItemHeight

                anchors {
                    horizontalCenter: parent.horizontalCenter
                    top: parent.top
                }

                anchors.topMargin: kbLayout.dragAnchorBottom ? kbLayout.desiredItemHeight : 0

                states: State {
                    when: kbLayoutVisualComponent.isDragging

                    ParentChange {
                        target: kbLayoutVisualComponent
                        parent: layoutSelectList
                    }
                    AnchorChanges {
                        target: kbLayoutVisualComponent
                        anchors.horizontalCenter: undefined
                        anchors.top: undefined
                        anchors.bottom: undefined
                    }
                }

                Rectangle {
                    id: selBg

                    anchors.fill: parent
                    radius: Config.appearance.rounding.sm

                    color: dragHandle.held ? Colors.colors.base2 : kbLayout.isActive ? Colors.colors.primary : "transparent"
                    opacity: dragHandle.held ? 1 : (kbLayout.isActive ? (root.enabled ? 1 : 0.5) : 0)
                    visible: opacity > 0

                    Behavior on opacity {
                        NAnim {
                            duration: 200
                        }
                    }
                }

                MouseArea {
                    id: dragHandle

                    readonly property int modelIndex: kbLayout.index
                    readonly property int padding: 2
                    property bool held: false

                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom

                    enabled: root.enabled
                    cursorShape: enabled ? Qt.OpenHandCursor : Qt.ArrowCursor

                    drag.target: held ? kbLayoutVisualComponent : undefined
                    drag.axis: Drag.YAxis

                    onPressed: held = true
                    onReleased: held = false

                    implicitWidth: dragIconWrapper.implicitWidth

                    Item {
                        id: dragIconWrapper

                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left

                        implicitWidth: parent.height

                        Rectangle {
                            anchors.fill: parent
                            anchors.margins: dragHandle.padding

                            color: dragHandle.held ? Colors.colors.base5 : Colors.colors.base3
                            radius: root.rounding - dragHandle.padding

                            border.width: 1
                            border.color: Colors.colors.base2
                        }

                        SMaterialIcon {
                            id: dragIcon

                            anchors.fill: parent

                            text: "drag_indicator"
                            font.pixelSize: height - dragHandle.padding * 4

                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }

                MouseArea {
                    anchors.left: dragHandle.right
                    anchors.leftMargin: Config.appearance.spacing.sm
                    anchors.right: itemRemoveIcon.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom

                    enabled: root.enabled
                    cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                    hoverEnabled: enabled

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
}
