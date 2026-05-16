import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick
import QtQuick.Controls

Flickable {
    id: root

    acceptedButtons: Qt.NoButton

    default property alias content: container.data
    readonly property bool isScrollBarActive: contentHeight > height
    property int scrollBarWidth: 8
    property int desiredHeight

    focus: true
    clip: true

    flickableDirection: Flickable.VerticalFlick
    boundsBehavior: Flickable.StopAtBounds
    contentHeight: Math.max(desiredHeight, container.childrenRect.height)

    ScrollBar.vertical: ScrollBar {
        policy: root.isScrollBarActive ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        padding: 0

        implicitWidth: root.scrollBarWidth

        contentItem: Rectangle {
            color: Colors.colors.primary
            radius: width / 2
        }

        background: Rectangle {
            color: Colors.colors.base2
            radius: width / 2
        }
    }

    Item {
        id: container

        anchors.fill: parent
        anchors.rightMargin: root.isScrollBarActive ? root.scrollBarWidth + Config.appearance.spacing.sm : 0

        Behavior on anchors.rightMargin {
            NAnim {}
        }
    }
}
