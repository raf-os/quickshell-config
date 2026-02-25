import QtQuick

Item {
    property string selectedPopoutId
    signal triggerPopout(item: Item, name: string)
    signal closePopout
}
