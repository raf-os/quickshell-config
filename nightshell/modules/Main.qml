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
			mask: Region {
				id: windowMask
				intersection: Intersection.Xor

				x: 0
				y: 32
				width: win.width
				height: win.height - y
			}

			anchors {
				top: true
				right: true
				bottom: true
				left: true
			}

			RootInteractionHandler {
				anchors.fill: parent

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

				Shapes.Main {
					id: bgShapes

					screen: scope.modelData
					bar: bar
				}
			}
		}
	}
}
