pragma Singleton

import Quickshell
import QtQuick

Singleton {
    id: root

    function launchDesktopApp(desktopEntry: DesktopEntry): void {
        if (desktopEntry.runInTerminal) {
            // handle running in terminal
        } else {
            Quickshell.execDetached({
                command: ["app2unit", "--", ...desktopEntry.command],
                workingDirectory: desktopEntry.workingDirectory
            });
        }
    }
}
