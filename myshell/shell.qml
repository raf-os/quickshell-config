import qs.modules
import qs.modules.lock
import qs.services
import Quickshell
import QtQuick

ShellRoot {
    Drawers {}
    HyprShortcuts {}
    Lock {}

    Component.onCompleted: {
        UserCommandService.setup();
    }
}
