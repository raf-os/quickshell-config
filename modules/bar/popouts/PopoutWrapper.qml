pragma ComponentBehavior: Bound

import qs.components
import qs.services
import qs.config
import Quickshell
import Quickshell.Wayland
import Quickshell.Hyprland
import QtQuick

Item {
    id: root

    required property ShellScreen screen

    readonly property real baseWidth: children.find(c => c.shouldBeActive)?.implicitWidth ?? content.implicitWidth
    readonly property real baseHeight: hasCurrent ? children.find(c => c.shouldBeActive)?.implicitHeight ?? content.implicitHeight : 0
    readonly property Item current: content.item?.current ?? null

    signal popoutChanged
    signal popoutClosed

    property string currentName: ""
    property real currentCenter
    property bool hasCurrent

    property int animLength: 400
    property list<real> animCurve: [0.05, 0, 2 / 15, 0.06, 1 / 6, 0.4, 5 / 24, 0.82, 0.25, 1, 1, 1]

    function toggle(item: Item, name: string, _currentCenter: real): string {
        if (hasCurrent && currentName === name) {
            close();
            popoutChanged();
            return undefined;
        } else {
            hasCurrent = true;
            currentName = name;
            currentCenter = _currentCenter;
            popoutChanged();
            return name;
        }
    }

    function close(): void {
        hasCurrent = false;
        popoutClosed();
    }

    visible: width > 0 && height > 0
    clip: true

    implicitWidth: baseWidth
    implicitHeight: baseHeight

    y: hasCurrent ? 4 : 0

    focus: hasCurrent

    Keys.onEscapePressed: {
        close();
    }

    StyledRect {
        id: background

        anchors.fill: parent

        color: ColorService.current.base
        radius: Config.appearance.rounding.sm
    }

    Comp {
        id: content

        shouldBeActive: root.hasCurrent
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        sourceComponent: PopoutContent {
            wrapper: root

            hasCurrent: root.hasCurrent
            currentName: root.currentName
        }
    }

    Behavior on implicitHeight {
        NAnim {
            duration: root.animLength
            easing.bezierCurve: root.animCurve
        }
    }

    Behavior on implicitWidth {
        enabled: root.implicitHeight > 0

        NAnim {
            duration: root.animLength
            easing.bezierCurve: root.animCurve
        }
    }

    Behavior on x {
        enabled: root.implicitHeight > 0
        NAnim {
            duration: root.animLength
            easing.bezierCurve: root.animCurve
        }
    }

    Behavior on y {
        NAnim {
            duration: root.animLength
            easing.bezierCurve: root.animCurve
        }
    }

    component Comp: Loader {
        id: comp

        property bool shouldBeActive

        active: false
        opacity: 0

        states: State {
            name: "active"
            when: comp.shouldBeActive

            PropertyChanges {
                comp.opacity: 1
                comp.active: true
            }
        }

        transitions: [
            Transition {
                from: ""
                to: "active"

                SequentialAnimation {
                    PropertyAction {
                        property: "active"
                    }
                    NAnim {
                        property: "opacity"
                    }
                }
            },
            Transition {
                from: "active"
                to: ""

                SequentialAnimation {
                    NAnim {
                        property: "opacity"
                    }
                    PropertyAction {
                        property: "active"
                    }
                }
            }
        ]
    }
}
