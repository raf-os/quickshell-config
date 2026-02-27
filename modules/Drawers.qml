pragma ComponentBehavior: Bound

import qs.components
import qs.config
import qs.modules.bar
import qs.services
import Quickshell
import Quickshell.Wayland
import Quickshell.Hyprland
import QtQuick
import QtQuick.Effects

Variants {
    model: Quickshell.screens

    Scope {
        id: scope

        required property ShellScreen modelData

        Exclusions {
            screen: scope.modelData
            bar: bar
        }

        BasePanel {
            id: win

            screen: scope.modelData
            name: "drawers"

            exclusionMode: ExclusionMode.Ignore

            mask: Region {
                x: Config.border.thickness
                y: bar.implicitHeight
                width: win.width - Config.border.thickness
                height: win.height - Config.border.thickness * 2

                intersection: Intersection.Xor

                regions: regions.instances
            }

            anchors {
                top: true
                bottom: true
                left: true
                right: true
            }

            Variants {
                id: regions

                model: panels.children

                Region {
                    required property Item modelData

                    x: modelData.x + Config.border.thickness
                    y: modelData.y + bar.implicitHeight
                    width: modelData.width
                    height: modelData.height

                    intersection: Intersection.Subtract
                }
            }

            StyledRect {
                anchors.fill: parent
                opacity: openPanels.session ? 0.5 : 0
                color: "black"

                Behavior on opacity {
                    NAnim {}
                }
            }

            Item {
                anchors.fill: parent
                layer.enabled: true
                layer.effect: MultiEffect {
                    shadowEnabled: true
                    blurMax: 32
                    shadowColor: Qt.alpha("black", 0.75)
                }

                Border {
                    bar: bar
                }

                PanelBackgrounds {
                    panels: panels
                    bar: bar
                }
            }

            PersistentProperties {
                id: openPanels

                property bool session

                Component.onCompleted: PanelService.load(scope.modelData, this)
            }

            HyprlandFocusGrab {
                id: focusHandler
                windows: [win]
                active: panels.popouts.hasCurrent
                onCleared: {
                    panels.popouts.close();
                }
            }

            InteractionHandler {
                screen: scope.modelData
                openPanels: openPanels
                popouts: panels.popouts
                bar: bar

                // implicitWidth: bar.implicitWidth
                // implicitHeight: bar.implicitHeight

                anchors.fill: parent
            }

            Panels {
                id: panels

                screen: scope.modelData
                openPanels: openPanels
                bar: bar
            }

            BarWrapper {
                id: bar

                anchors.left: parent.left
                anchors.right: parent.right

                screen: scope.modelData
                openPanels: openPanels
                popouts: panels.popouts

                Component.onCompleted: PanelService.bars.set(scope.modelData, this)
            }
        }
    }
}
