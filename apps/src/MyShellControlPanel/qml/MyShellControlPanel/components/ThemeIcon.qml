import QtQuick

Item {
    id: root

    required property string iconName
    property int size: 16
    property alias backer: img

    implicitWidth: size
    implicitHeight: size

    Image {
        id: img

        source: `image://qicons/${root.iconName}`

        anchors.centerIn: parent

        width: root.size
        height: root.size
    }
}
