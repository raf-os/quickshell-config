import qs.config
import qs.services
import qs.utils
import Quickshell
import QtQuick

Item {
    id: root

    required property ShellScreen screen
    required property Item bar
    required property PersistentProperties openPanels
    property bool isActive: false

    anchors.fill: parent
    anchors.margins: Config.border.thickness
    anchors.topMargin: bar.implicitHeight

    Timer {
        id: dismissTimer

        interval: 3000

        onTriggered: {
            osdContent?.triggerExitAnim();
        }
    }

    Timer {
        id: initialDelay
        running: true

        interval: 500

        onTriggered: {
            root.isActive = true;
        }
    }

    function triggerOSD(type: string) {
        if (!root.isActive)
            return;
        const isActive = Hypr.focusedMonitor.name === root.screen.name;
        if (!isActive) {
            dismissTimer.stop();
            osdContent?.hide();
            return;
        } else {
            osdContent?.show(type);
            dismissTimer.restart();
        }
    }

    Connections {
        target: MprisService

        enabled: Config.media.enabled

        function onTriggerOsd() {
            const currentPlayer = MprisService.currentActive;
            if (currentPlayer === null)
                return;

            root.triggerOSD("mprischange");
        }
    }

    Connections {
        target: AudioService

        function onIsMutedChanged() {
            if (AudioService.isMuted)
                root.triggerOSD("mute");
            else
                root.triggerOSD("volumechange");
        }

        function onVolumeChanged() {
            if (!AudioService.isMuted) {
                root.triggerOSD("volumechange");
            }
        }
    }

    Connections {
        target: GlobalStateManager

        function onIsGameModeChanged() {
            root.triggerOSD("gamemode");
        }
    }

    Loader {
        id: osdSlot

        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        active: true

        Content {
            id: osdContent
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
