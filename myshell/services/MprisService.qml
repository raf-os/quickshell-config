pragma Singleton

import qs.config
import Quickshell
import Quickshell.Services.Mpris
import QtQuick

Singleton {
    id: root

    readonly property list<MprisPlayer> playerList: Mpris.players.values
    readonly property MprisPlayer currentActive: props.activeOverride ?? playerList.find(p => resolvePlayerAlias(p) === Config.media.defaultPlayer) ?? null
    property alias activeOverride: props.activeOverride

    property bool triggerPositionUpdateFlag

    function resolvePlayerAlias(player: MprisPlayer): string {
        const alias = Config.media.playerAliases.find(a => a.from === player.identity);
        return alias?.to ?? player.identity;
    }

    function getArtUrl(player: MprisPlayer): string {
        if (!player) {
            if (currentActive !== null) {
                return getArtUrl(currentActive);
            }
            return "";
        }
        if (player.trackArtUrl)
            return player.trackArtUrl;
        return "";
    }

    function updateTrackPositions(): void {
        // Batches all calls to update track positions
        // into a single one, so every component that
        // attempts to read the position doesn't trigger
        // an update themselves
        if (triggerPositionUpdateFlag)
            return;

        triggerPositionUpdateFlag = true;
    }

    signal trackChanged
    signal playbackStateChanged
    signal triggerOsd

    FrameAnimation {
        running: root.triggerPositionUpdateFlag

        onTriggered: {
            // NOTE: for ... in javascript loop did not work. Framework issue or skill issue?
            for (let i = 0; i < root.playerList.length; i++) {
                if (root.playerList[i]?.positionSupported === true && root.playerList[i]?.isPlaying === true) {
                    root.playerList[i].positionChanged();
                }
            }
            root.triggerPositionUpdateFlag = false;
        }
    }

    Connections {
        target: root.currentActive

        function onPostTrackChanged() {
            root.trackChanged();
            if (root.currentActive?.playbackState === MprisPlaybackState.Playing) {
                root.triggerOsd();
            }
        }

        function onPlaybackStateChanged() {
            root.playbackStateChanged();

            if (root.currentActive?.playbackState === MprisPlaybackState.Playing) {
                root.triggerOsd();
            }
        }
    }

    PersistentProperties {
        id: props

        property MprisPlayer activeOverride

        reloadableId: "mprisPlayersProps"
    }
}
