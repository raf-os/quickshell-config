import QtQuick

Item {
    property string selectedPopoutId
    signal triggerPopout(item: Item, name: string)
    signal closePopout

    signal triggerTooltip(item: Item, content: string)
    signal closeTooltip
}
