pragma ComponentBehavior: Bound

import qs.components
import qs.shapes as Shapes
import qs.modules.bar

import Quickshell
import Quickshell.Wayland
import QtQuick

Variants {
	id: root

	model: Quickshell.screens

	Scope {
		id: scope
		required property ShellScreen modelData

		Exclusions {
			screen: scope.modelData
			bar: bar
		}

		BasePanelWindow {
			id: win

			name: "main-shell"
			screen: scope.modelData

			exclusionMode: ExclusionMode.Ignore
			mask: MaskRegions {
				id: windowMask

				bar: bar
				win: win
			}

			anchors {
				top: true
				right: true
				bottom: true
				left: true
			}

			RootInteractionHandler {
				anchors.fill: parent

				Shapes.Main {
					id: bgShapes

					screen: scope.modelData
					bar: bar
				}

				Bar {
					id: bar
					screen: scope.modelData
					panelWindow: win

					anchors {
						top: parent.top
						left: parent.left
						right: parent.right
					}
				}

				Content {
					id: content
					screen: scope.modelData
					win: win
					bar: bar

					anchors {
						top: bar.bottom
						left: parent.left
						right: parent.right
						bottom: parent.bottom
					}
				}
			}
		}
	}
}
