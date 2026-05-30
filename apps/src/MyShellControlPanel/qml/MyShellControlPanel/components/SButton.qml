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
    property bool focusable: true

    property color hoverColor: Colors.colors.primary2
    property color baseColor: Colors.colors.primary

    signal activated(event: MouseEvent)

    implicitWidth: autoWidth ? 0 : btnText.width
    implicitHeight: autoHeight ? 0 : btnText.height

    hoverEnabled: true
    cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
    enabled: !disabled

    onEnabledChanged: {
        if (!enabled) {
            focus = false;
            activeFocusOnTab = false;
        } else {
            activeFocusOnTab = focusable;
        }
    }

    Component.onCompleted: {
        evalColors();
        root.activeFocusOnTab = enabled && focusable;
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

    onClicked: event => {
        root.activated(event);
    }

    Keys.onReturnPressed: {
        if (!enabled)
            return;

        root.activated(null);
    }

    Rectangle {
        id: bgRect

        anchors.fill: parent
        radius: root.radius

        color: (root.enabled) ? (root.containsMouse) ? root.hoverColor : root.baseColor : Colors.colors.base2
    }

    Rectangle {
        id: focusRect

        z: -1

        anchors.fill: parent
        anchors.margins: -2
        radius: root.radius + 2

        color: Colors.colors.neutralContent
        visible: root.enabled
        opacity: root.activeFocus ? 1 : 0

        // border.width: 2
        // border.color: Colors.colors.neutralContent
    }

    StyledText {
        id: btnText

        text: root.text
        padding: root.padding
        anchors.centerIn: parent

        font.family: root.fontFamily
        font.pointSize: root.fontSize

        opacity: root.enabled ? 1 : 0.75
    }
}
