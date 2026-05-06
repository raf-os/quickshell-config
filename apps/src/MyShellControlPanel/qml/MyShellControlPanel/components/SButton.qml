import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick

MouseArea {
    id: root

    enum ButtonType {
        Default,
        Destructive
    }

    required property string text
    property int type: SButton.ButtonType.Default
    property int radius: Config.appearance.rounding.sm
    property bool disabled: false
    property string fontFamily: Config.appearance.fontFamily.sans
    property int fontSize: Config.appearance.fontSize.xs
    property int padding: Config.appearance.padding.sm

    property bool autoWidth: false
    property bool autoHeight: false

    property color hoverColor: Colors.colors.primary2
    property color baseColor: Colors.colors.primary

    implicitWidth: autoWidth ? 0 : btnText.width
    implicitHeight: autoHeight ? 0 : btnText.height

    hoverEnabled: true
    cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
    enabled: !disabled

    Component.onCompleted: {
        evalColors();
    }

    onTypeChanged: {
        evalColors();
    }

    function evalColors() {
        switch (root.type) {
        case SButton.ButtonType.Default:
            root.hoverColor = Colors.colors.primary2;
            root.baseColor = Colors.colors.primary;
            break;
        case SButton.ButtonType.Destructive:
            root.hoverColor = Colors.colors.destructiveHover;
            root.baseColor = Colors.colors.destructive;
            break;
        }
    }

    Rectangle {
        id: bgRect

        anchors.fill: parent
        radius: root.radius

        color: (root.enabled) ? (root.containsMouse) ? root.hoverColor : root.baseColor : Colors.colors.base2
    }

    StyledText {
        id: btnText

        text: root.text
        padding: root.padding
        anchors.centerIn: parent

        font.family: root.fontFamily
        font.pointSize: root.fontSize

        opacity: root.disabled ? 0.75 : 1
    }
}
