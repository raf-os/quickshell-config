import MyShellControlPanel.components
import MyShellPlugin
import QtQuick

Item {
    id: root

    implicitWidth: 320

    Rectangle {
        anchors.fill: parent
        color: Colors.colors.base2
    }

    Item {
        id: titleLabel
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        implicitHeight: 32

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: "Control panel"
        }
    }

    ListView {
        id: menuItems

        anchors.top: titleLabel.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        model: SidebarModel {}

        delegate: Item {
            id: listItem
            required property string label

            Text {
                text: listItem.label
                color: Colors.colors.baseContent
            }
        }
    }
}
