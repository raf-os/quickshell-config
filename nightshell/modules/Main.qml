pragma ComponentBehavior: Bound

import org.nightshell.Utils
import org.nightshell.Hyprland as N_HYPRLAND

import qs.components
import qs.shapes as Shapes
import qs.modules.bar
import qs.utils

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

			WlrLayershell.keyboardFocus: FocusGrabberManager.active ? WlrKeyboardFocus.Exclusive : WlrKeyboardFocus.OnDemand

			exclusionMode: ExclusionMode.Ignore
			mask: MaskRegions {
				id: windowMask

				bar: bar
				win: win
				content: content
			}

			anchors {
				top: true
				right: true
				bottom: true
				left: true
			}

			InstanceContext {
				id: context

				hyprMonitor: N_HYPRLAND.Hyprland.monitorsModel.values.find(m => m.name === scope.modelData.name)
				win: win
				shellScreen: scope.modelData
				bar: bar
				content: content // qmllint disable incompatible-type
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
					panelWindow: win
					context: context

					anchors {
						top: parent.top
						left: parent.left
						right: parent.right
					}
				}

				Content {
					id: content
					context: context

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
