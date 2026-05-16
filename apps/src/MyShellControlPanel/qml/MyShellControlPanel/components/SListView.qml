import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick
import QtQuick.Controls

Item {
    id: root

    property Component delegate: null
    required property var model

    clip: true

    ListView {
        id: listView

        acceptedButtons: Qt.NoButton

        readonly property bool isScrollBarActive: contentHeight > height
        readonly property int scrollBarWidth: 8
        readonly property int scrollBarClearance: scrollBarWidth + Config.appearance.spacing.sm

        boundsBehavior: Flickable.StopAtBounds
        anchors.fill: parent
        anchors.rightMargin: scrollBarClearance

        ScrollBar.vertical: null

        model: root.model
        delegate: root.delegate
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
