pragma ComponentBehavior: Bound

import qs.components
import qs.config
import qs.services
import qs.utils
import QtQuick

Item {
    id: root

    property bool isActive: GlobalStateManager.isGameMode
    readonly property int animDuration: 200

    implicitWidth: Config.appearance.fontSize.xl
    // implicitHeight: Config.bar.sizes.innerHeight

    anchors.top: parent.top
    anchors.bottom: parent.bottom

    function checkGamemodeState(callback: var): void {
        GlobalStateManager.checkGamemodeState(callback);
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        acceptedButtons: Qt.LeftButton

        onClicked: event => {
            if (event.button === Qt.LeftButton) {
                GlobalStateManager.toggleGamemodeState();
            }
        }
    }

    Rectangle {
        anchors.centerIn: parent

        readonly property int size: Config.appearance.fontSize.xl

        implicitWidth: size
        implicitHeight: size

        color: ColorService.current.primary
        radius: Config.appearance.rounding.xs

        opacity: root.isActive ? 1 : 0

        Behavior on opacity {
            NAnim {
                duration: 300
            }
        }
    }

    StyledText {
        id: icon
        anchors.fill: parent
        text: root.isActive ? "󰖺" : "󰖻"
        color: root.isActive ? ColorService.current.baseContent : ColorService.current.baseContentMuted

        font.pixelSize: Config.appearance.fontSize.xxl
        font.family: Config.appearance.fontFamily.monoIcon

        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter

        Behavior on color {
            CAnim {
                duration: root.animDuration
            }
        }
    }
}
