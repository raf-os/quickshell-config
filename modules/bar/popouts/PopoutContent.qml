pragma ComponentBehavior: Bound

import qs.components
import qs.config
import Quickshell
import Quickshell.DBusMenu
import Quickshell.Services.SystemTray
import QtQuick

Item {
    id: root

    required property Item wrapper
    required property bool hasCurrent
    required property string currentName

    readonly property Popout currentPopout: content.children.find(c => c.shouldBeActive) ?? null
    readonly property Item current: currentPopout?.item ?? null

    anchors.centerIn: parent

    implicitWidth: (currentPopout?.implicitWidth ?? 0) + Config.appearance.padding.sm * 2
    implicitHeight: (currentPopout?.implicitHeight ?? 0) + Config.appearance.padding.xs * 2

    Item {
        id: content

        anchors.fill: parent
        anchors.margins: Config.appearance.padding.sm
        anchors.topMargin: Config.appearance.padding.xs
        anchors.bottomMargin: Config.appearance.padding.xs

        Popout {
            name: "audio"
            sourceComponent: AudioPopout {
                wrapper: root.wrapper
            }
        }

        Repeater {
            model: ScriptModel {
                values: [...SystemTray.items.values]
            }

            Popout {
                id: trayMenu

                required property SystemTrayItem modelData
                required property int index

                name: `traymenu${index}`
                sourceComponent: trayMenuComp

                Connections {
                    target: root.wrapper

                    function onHasCurrentChanged(): void {
                        if (root.hasCurrent && trayMenu.shouldBeActive) {
                            trayMenu.sourceComponent = null;
                            trayMenu.sourceComponent = trayMenuComp;
                        }
                    }
                }

                Component {
                    id: trayMenuComp

                    Loader {
                        active: trayMenu.modelData !== null
                        sourceComponent: TrayMenu {
                            popouts: root.wrapper
                            trayItem: trayMenu.modelData.menu
                        }
                    }
                }
            }
        }
    }

    component Popout: Loader {
        id: popout

        required property string name
        readonly property bool shouldBeActive: root.currentName === name

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom

        opacity: 0
        active: false

        states: State {
            name: "activated"
            when: popout.shouldBeActive

            PropertyChanges {
                popout.active: true
                popout.opacity: 1
            }
        }

        transitions: [
            Transition {
                from: "activated"
                to: ""

                SequentialAnimation {
                    NAnim {
                        property: "opacity"
                        duration: 400
                    }
                    PropertyAction {
                        target: popout
                        property: "active"
                        value: false
                    }
                }
            },
            Transition {
                from: ""
                to: "activated"

                SequentialAnimation {
                    PropertyAction {
                        target: popout
                        property: "active"
                        value: true
                    }
                    NAnim {
                        property: "opacity"
                        duration: 400
                    }
                }
            }
        ]
    }
}
