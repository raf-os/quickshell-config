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

    function resolvePlayerAlias(player: MprisPlayer): string {
        const alias = Config.media.playerAliases.find(a => a.from === player.identity);
        return alias?.to ?? player.identity;
    }

    signal trackChanged
    signal playbackStateChanged

    Connections {
        target: root.currentActive

        function onPostTrackChanged() {
            root.trackChanged();
        }

        function onPlaybackStateChanged() {
            root.playbackStateChanged();
        }
    }

    PersistentProperties {
        id: props

        property MprisPlayer activeOverride

        reloadableId: "mprisPlayersProps"
    }
}
