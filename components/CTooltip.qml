import qs.modules.bar
import QtQuick

Item {
    id: root

    required property MouseArea mouseArea
    required property PopoutHandler popoutHandler
    required property string tooltipText

    anchors.fill: parent

    Connections {
        target: root.mouseArea

        function onEntered() {
            root.popoutHandler.triggerTooltip(root, root.tooltipText);
        }

        function onExited() {
            root.popoutHandler.closeTooltip();
        }
    }
}
