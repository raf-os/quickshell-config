import qs.modules
import qs.modules.lock
import qs.services
import MyShellPlugin
import MyShellPlugin.Configs
import Quickshell
import QtQuick

ShellRoot {
    Drawers {}
    HyprShortcuts {}
    Lock {}

    Component.onCompleted: {
        UserCommandService.setup();

        // Config.earlyLoad();
        // Colors.earlyLoad();
    }
}
