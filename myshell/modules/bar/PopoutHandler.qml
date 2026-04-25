import Quickshell
import QtQuick

Item {
    required property PersistentProperties openPanels

    property string selectedPopoutId
    signal triggerPopout(item: Item, name: string)
    signal closePopout

    signal triggerTooltip(item: Item, content: string)
    signal closeTooltip
}
