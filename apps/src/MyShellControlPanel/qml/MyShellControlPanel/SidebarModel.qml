import QtQuick

ListModel {
    id: root

    ListElement {
        label: "Keyboard"
        path: "/keyboard"
        icon: "input-keyboard-symbolic"
    }

    ListElement {
        label: "Appearance"
        path: "/appearance"
        icon: "draw-watercolor-symbolic"
    }

    ListElement {
        label: "Bar Config"
        path: "/taskbar"
        icon: "configure-toolbars-symbolic"
    }

    ListElement {
        label: "Colors"
        path: "/colors"
        icon: "color-picker-white"
    }
}
