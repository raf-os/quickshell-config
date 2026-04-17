import QtQuick
import QtQuick.Controls

import "qml"

Window {
    id: root
    visible: true

    Sidebar {
        id: sidebar

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
    }

    StackView {
        id: mainPage

        anchors.left: sidebar.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
    }
}
