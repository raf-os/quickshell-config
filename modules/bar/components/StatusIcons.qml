pragma ComponentBehavior: Bound

import "statusIcons"
import qs.components
import qs.modules.bar
import qs.services.network
import qs.utils
import qs.services
import qs.config
import QtQuick
import QtQuick.Layouts

Item {
    id: root

    required property PopoutHandler popoutHandler

    implicitWidth: iconRow.implicitWidth + Config.appearance.padding.xl
    implicitHeight: Config.bar.sizes.innerHeight

    StyledRect {
        id: bgRect

        anchors.fill: parent

        color: ColorService.current.base0
        radius: Config.appearance.rounding.sm
    }

    RowLayout {
        id: iconRow

        anchors.centerIn: parent

        spacing: Config.appearance.spacing.xs

        WrappedLoader {
            id: gameModeStatus
            name: "Game mode"
            active: true

            sourceComponent: GameModeIcon {}
        }

        WrappedLoader {
            id: notifStatus
            name: "Notifications"
            active: true

            sourceComponent: NotificationsIcon {}
        }

        MouseArea {
            implicitWidth: audioStatus.implicitWidth
            implicitHeight: audioStatus.implicitHeight
            cursorShape: Qt.PointingHandCursor

            WrappedLoader {
                id: audioStatus
                name: "Audio settings"
                active: true

                sourceComponent: AudioStatusIcon {}
            }

            onClicked: ev => {
                root.popoutHandler.triggerPopout(audioStatus, "audio");
            }
        }

        WrappedLoader {
            id: networkStatus
            name: "Network settings"
            active: true

            sourceComponent: NetworkStatusIcon {
                text: Networkctl.isConnected ? "󰱓" : "󰅛"
            }
        }
    }

    component WrappedLoader: Loader {
        required property string name

        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        visible: active
    }
}
