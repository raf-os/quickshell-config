pragma Singleton
pragma ComponentBehavior: Bound

import qs.services
import MyShellPlugin
import MyShellPlugin.Configs
import Quickshell
import QtQuick

Singleton {
    id: root

    readonly property bool isEnabled: Config.sounds.enabled

    function setup() {
    }

    Loader {
        id: systemSoundsToggler

        active: root.isEnabled

        sourceComponent: SystemSounds {
            id: systemSounds

            eventHandler: Hypr.hyprEvents
        }
    }
}
