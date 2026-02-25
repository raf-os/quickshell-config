pragma Singleton

import Quickshell
import Quickshell.Io
import QtQuick

Singleton {
    id: root

    property string osName
    property string osId
    property string osLogo: Qt.resolvedUrl(`${Quickshell.shellDir}/assets/os-logo.svg`)

    readonly property string user: Quickshell.env("USER")

    FileView {
        id: osRelease

        path: "/etc/os-release"
        onLoaded: {
            const lines = text().split("\n");
            const fd = key => lines.find(l => l.startsWith(`${key}=`))?.split("=")[1].replace(/"/g, "") ?? ""; //"

            root.osName = fd("Name");
            root.osId = fd("ID");

            const logo = Quickshell.iconPath(fd("LOGO"), true);
            root.osLogo = logo;
        }
    }
}
