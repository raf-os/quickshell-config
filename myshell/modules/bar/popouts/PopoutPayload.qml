import qs.modules.bar
import Quickshell
import QtQuick

// Attach to tray items
Item {
    id: root

    required property string name
    required property PopoutHandler popoutHandler

    anchors.fill: parent

    function triggerPayload() {
        popoutHandler.triggerPopout(parent);
    }
}
