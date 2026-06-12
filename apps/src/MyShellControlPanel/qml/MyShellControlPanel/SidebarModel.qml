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
        icon: "appearance"
    }

    ListElement {
        label: "Launcher"
        path: "/launcher"
        icon: "launcher"
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

    ListElement {
        label: "Media"
        path: "/media"
        icon: "media"
    }

    ListElement {
        label: "System Sounds"
        path: "/sounds"
        icon: "media"
    }

    ListElement {
        label: "Not Found"
        path: "/404"
        icon: ""
        dummyEntry: true
    }
}
