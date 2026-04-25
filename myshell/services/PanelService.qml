pragma Singleton

import Quickshell

Singleton {
    property var screens: new Map()
    property var bars: new Map()

    function load(screen: ShellScreen, openPanels: var): void {
        screens.set(Hypr.monitorFor(screen), openPanels);
    }

    function getForActive(): PersistentProperties {
        return screens.get(Hypr.focusedMonitor);
    }
}
