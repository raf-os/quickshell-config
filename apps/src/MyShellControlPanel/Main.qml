import MyShellPlugin
import QtQuick
import QtQuick.Controls

import "qml"

Window {
    id: root
    visible: true

    color: Colors.colors.base
    objectName: "Main"

    width: 800
    height: 640

    minimumWidth: 640
    minimumHeight: 320

    title: "MyShell Control Panel"

    App {}
}
