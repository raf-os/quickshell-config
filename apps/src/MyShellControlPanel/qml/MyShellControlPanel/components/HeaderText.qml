import MyShellPlugin.Configs
import MyShellControlPanel.components
import QtQuick

Item {
    id: root

    required property string text
    required property int level

    implicitHeight: header.height

    QtObject {
        id: private

        property int pointSize: {
            switch (root.level) {
            case 1:
                return Config.appearance.fontSize.xl;
            case 2:
                return Config.appearance.fontSize.lg;
            case 3:
                return Config.appearance.fontSize.md;
            default:
                return Config.appearance.fontSize.sm;
            }
        }

        property int weight: {
            switch (root.level) {
            case 1:
            case 2:
                return 700;
            case 3:
                return 600;
            default:
                return 500;
            }
        }
    }

    StyledText {
        id: header

        text: root.text

        font.pointSize: private.pointSize
        font.family: Config.appearance.fontFamily.sans
        font.weight: private.weight

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
    }
}
