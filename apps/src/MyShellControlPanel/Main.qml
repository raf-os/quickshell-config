import QtQuick
import "qml"

Window {
    id: root
    visible: true

    Sidebar {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
    }
}
