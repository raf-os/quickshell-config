pragma ComponentBehavior: Bound

import qs.components
import qs.components.controls
import qs.services
import qs.config
import Quickshell.Services.Pipewire
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    required property var wrapper

    implicitWidth: layout.implicitWidth + Config.appearance.padding.md * 2
    implicitHeight: layout.implicitHeight + Config.appearance.padding.sm * 2

    ColumnLayout {
        id: layout

        anchors.left: parent.left
        anchors.centerIn: parent
        spacing: Config.appearance.spacing.md

        StyledText {
            // Layout.topMargin: Config.appearance.spacing.xs
            Layout.bottomMargin: -Config.appearance.spacing.sm
            text: qsTr("Volume (%1)").arg(AudioService.isMuted ? qsTr("Muted") : `${Math.round(AudioService.volume * 100)}%`)
            font.weight: 500
        }

        MouseArea {
            Layout.fillWidth: true
            implicitWidth: 200
            implicitHeight: 16

            StyledSlider {
                anchors.left: parent.left
                anchors.right: parent.right
                implicitHeight: parent.implicitHeight

                value: AudioService.volume
                onMoved: AudioService.setVolume(Math.round(value * 100) / 100)

                Behavior on value {
                    NAnim {}
                }
            }
        }
    }
}
