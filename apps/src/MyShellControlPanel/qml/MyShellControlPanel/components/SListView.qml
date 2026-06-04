import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick
import QtQuick.Controls

Item {
    id: root

    property alias listView: listView
    property Component delegate: null
    property Component hightlight: null
    property bool highlightFollowsCurrentItem: true
    required property var model

    property alias spacing: listView.spacing
    property alias currentIndex: listView.currentIndex
    property alias currentItem: listView.currentItem
    property alias highlightItem: listView.highlightItem
    property alias highlightMoveDuration: listView.highlightMoveDuration
    property alias highlightResizeDuration: listView.highlightResizeDuration
    property alias highlightResizeVelocity: listView.highlightResizeVelocity
    property alias highlightRangeMode: listView.highlightRangeMode
    property alias preferredHighlightBegin: listView.preferredHighlightBegin
    property alias preferredHighlightEnd: listView.preferredHighlightEnd
    property alias keyNavigationEnabled: listView.keyNavigationEnabled
    property alias keyNavigationWraps: listView.keyNavigationWraps

    ListView {
        id: listView

        acceptedButtons: Qt.NoButton

        readonly property bool isScrollBarActive: contentHeight > height
        readonly property int scrollBarWidth: 8
        readonly property int scrollBarClearance: isScrollBarActive ? scrollBarWidth + Config.appearance.spacing.sm : 0

        clip: true
        boundsBehavior: Flickable.StopAtBounds
        anchors.fill: parent
        anchors.rightMargin: scrollBarClearance

        ScrollBar.vertical: null

        model: root.model
        delegate: root.delegate
        highlight: root.hightlight
        highlightFollowsCurrentItem: root.highlightFollowsCurrentItem
    }

    ScrollBar {
        policy: listView.isScrollBarActive ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff

        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        padding: 0

        size: listView.visibleArea.heightRatio
        position: listView.visibleArea.yPosition

        onPositionChanged: {
            if (active) {
                listView.contentY = position * listView.contentHeight;
            }
        }

        implicitWidth: listView.scrollBarWidth

        contentItem: Rectangle {
            color: Colors.colors.primary
            radius: width / 2
        }

        background: Rectangle {
            color: Colors.colors.base2
            radius: width / 2
        }
    }
}
