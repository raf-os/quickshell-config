import qs.modules
import qs.services
import Quickshell
import QtQuick

ShellRoot {
    Drawers {}
    HyprShortcuts {}

    Component.onCompleted: {
        UserCommandService.setup();
    }
}
