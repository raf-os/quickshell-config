pragma Singleton

import qs.config
import Quickshell
import QtQuick

Singleton {
    id: root

    function getAppIcon(name: string, fallback: string): string {
        const icon = DesktopEntries.heuristicLookup(name)?.icon;
        if (fallback !== "undefined")
            return Quickshell.iconPath(icon, fallback);
        return Quickshell.iconPath(icon);
    }

    function getAppCategoryIcon(name: string, fallback: string): string {
        const categories = DesktopEntries.heuristicLookup(name)?.categories;

        if (categories)
            for (const [key, value] of Object.entries(categoryIcons))
                if (categories.includes(key))
                    return value;
        return fallback;
    }

    function getTrayIcon(id: string, icon: string): string {
        let parsedIcon = icon;
        if (parsedIcon.includes("?path=")) {
            const [name, path] = icon.split("?path=");
            parsedIcon = Qt.resolvedUrl(`${path}/${name.slice(name.lastIndexOf("/") + 1)}`);
        }
        return parsedIcon;
    }

    function getVolumeIcon(volume: real, isMuted: bool): string {
        if (isMuted)
            return "󰝟";
        else if (volume >= 0.5)
            return "󰕾";
        else if (volume > 0)
            return "󰖀";
        else
            return "󰖁";
    }
}
