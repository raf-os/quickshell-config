pragma ComponentBehavior: Bound

import qs.config
import qs.services
import qs.components
import qs.modules.bar
import Quickshell
import QtQuick

Item {
    id: root

    required property PopoutHandler popoutHandler
    required property string currentPopoutName

    property string currentLayout: Hypr.currentLayout.slice(0, 2)

    implicitWidth: Config.appearance.fontSize.xl
    // implicitHeight: Config.bar.sizes.innerHeight

    function setCurrentLayout(code: string): void {
        currentLayout = code.slice(0, 2);
    }

    Rectangle {
        id: hoverBg

        readonly property bool shouldBeActive: root.currentPopoutName === "language"

        anchors.centerIn: parent

        implicitWidth: parent.width
        implicitHeight: implicitWidth

        radius: Config.appearance.rounding.xs
        color: ColorService.current.base3

        opacity: shouldBeActive ? 1 : 0

        Behavior on opacity {
            NAnim {
                duration: 300
            }
        }
    }

    StyledText {
        id: displayText
        anchors.fill: parent

        text: root.currentLayout
        font.family: Config.appearance.fontFamily.sans
        font.weight: 600
        font.pointSize: Config.appearance.fontSize.xxs

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter

        opacity: Hypr.isKeyboardSwitchOnCooldown ? 0.5 : 1
    }

    MouseArea {
        id: interactionArea

        anchors.fill: parent

        cursorShape: Qt.PointingHandCursor
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        hoverEnabled: true

        onClicked: event => {
            if (event.button === Qt.LeftButton) {
                Hypr.switchKeyboardLayout("next");
                root.popoutHandler.closePopout();
            } else if (event.button === Qt.RightButton) {
                root.popoutHandler.triggerPopout(this, "language");
            }
        }
    }
}
