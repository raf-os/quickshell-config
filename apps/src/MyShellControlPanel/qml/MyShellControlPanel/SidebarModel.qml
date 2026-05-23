import QtQuick

ListModel {
    id: root

    ListElement {
        label: "Keyboard"
        path: "/keyboard"
        icon: "keyboard"
    }

    ListElement {
        label: "Appearance"
        path: "/appearance"
        icon: "image"
    }

    ListElement {
        label: "Bar Config"
        path: "/taskbar"
        icon: "toolbar"
    }

    ListElement {
        label: "Colors"
        path: "/colors"
        icon: "palette"
    }
}
