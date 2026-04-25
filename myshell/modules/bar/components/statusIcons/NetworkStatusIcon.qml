import qs.config
import qs.components
import MyShellPlugin
import QtQuick

Item {
    id: root

    readonly property bool isConnected: NetworkService.isConnected
    readonly property bool isConnecting: NetworkService.isConnecting
    readonly property bool isLimited: NetworkService.isLimitedConnection

    readonly property string text: {
        if (isConnected)
            return "󰱓";
        else if (isConnecting)
            return "󱂇";
        else if (isLimited)
            return "󰛵";
        else
            return "󰅛";
    }

    implicitWidth: Config.appearance.fontSize.xl
    // implicitHeight: Config.bar.sizes.innerHeight

    StyledText {
        id: icon
        anchors.fill: parent
        text: root.text
        font.pixelSize: Config.appearance.fontSize.xl
        font.family: Config.appearance.fontFamily.monoIcon

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
