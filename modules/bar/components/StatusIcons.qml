pragma ComponentBehavior: Bound

import "statusIcons"
import qs.components
import qs.modules.bar
import qs.services.network
import qs.utils
import qs.config
import QtQuick
import QtQuick.Layouts

StyledRect {
    id: root

    required property PopoutHandler popoutHandler

    implicitWidth: iconRow.implicitWidth + Config.appearance.padding.md
    implicitHeight: Config.bar.sizes.innerHeight

    RowLayout {
        id: iconRow

        anchors.fill: parent
        spacing: Config.appearance.spacing.sm

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
