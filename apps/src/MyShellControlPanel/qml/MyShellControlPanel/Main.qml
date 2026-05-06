import MyShellPlugin
import QtQuick

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

    required property string desiredInitialPath

    App {
        desiredInitialPath: root.desiredInitialPath
    }
}
