import QtQuick

Item {
    id: root

    anchors.fill: parent

    required property var lock

    AuthBox {
        lock: root.lock
    }
}
