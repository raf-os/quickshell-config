pragma Singleton

import qs.config
import qs.utils
import MyShellPlugin
import Quickshell
import QtQuick

Singleton {
    id: root

    property list<QtObject> list: appDb.apps
    property list<QtObject> filteredApps: appDb.filteredApps

    function query(q: string): list<var> {
        return appDb.queryApps(q);
    }

    function launchDesktopApp(desktopEntry: DesktopEntry): void {
        appDb.incrementFrequency(desktopEntry.id);

        if (desktopEntry.runInTerminal) {
            // handle running in terminal
        } else {
            Quickshell.execDetached({
                command: ["app2unit", "--", ...desktopEntry.command],
                workingDirectory: desktopEntry.workingDirectory
            });
        }
    }

    AppDb {
        id: appDb

        path: `${Paths.state}/apps.sqlite`
        favoriteApps: []
        entries: DesktopEntries.applications.values
    }
}
