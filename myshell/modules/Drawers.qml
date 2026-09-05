pragma ComponentBehavior: Bound

import qs.components
import qs.modules.bar
import qs.modules.osd as OSDWrapper
import qs.services
import qs.utils
import MyShellPlugin
import MyShellPlugin.Configs
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
			WlrLayershell.keyboardFocus: openPanels.startmenu || openPanels.notifications ? WlrKeyboardFocus.OnDemand : WlrKeyboardFocus.None

			mask: MaskRegions {
				bar: bar
				panels: panels
				win: win
			}

			anchors {
				top: true
				bottom: true
				left: true
				right: true
			}

			Component.onCompleted: {
				GlobalShellState.registerWindowToScreen(win, scope.modelData);
			}

			Component.onDestruction: {
				GlobalShellState.unregisterWindow(win);
			}

			// Variants {
			// 	id: regions
			//
			// 	model: panels.exclusions
			//
			// 	delegate: Region {
			// 		required property Item modelData
			//
			// 		x: modelData.x + Config.border.thickness
			// 		y: modelData.y + bar.implicitHeight
			// 		width: modelData.width
			// 		height: modelData.height
			//
			// 		intersection: Intersection.Subtract
			// 	}
			// }

			StyledRect {
				anchors.fill: parent
				opacity: openPanels.session | GlobalShellState.isLauncherOpen ? 0.5 : 0
				color: "black"

				Behavior on opacity {
					NAnim {}
				}
			}

			Item {
				anchors.fill: parent
				layer.enabled: !GlobalStateManager.isGameMode
				layer.effect: MultiEffect {
					visible: !GlobalStateManager.isGameMode
					autoPaddingEnabled: false
					shadowEnabled: true

					// Using shadow scale to mimic shadow "spread"
					// Can potentially look wonky on screens with
					// absurd width/height ratios
					shadowScale: 0.998
					blurMax: 12
					shadowColor: Qt.rgba(0, 0, 0, 1.0)
					shadowOpacity: 0.7
					shadowVerticalOffset: 2
				}

				Border {
					bar: bar
				}

				PanelBackgrounds {
					panels: panels
					bar: bar
				}
			}

			OpenPanels {
				id: openPanels
				screen: scope.modelData
			}

			HyprlandFocusGrab {
				id: focusHandler
				windows: [win]
				active: panels.popouts.hasCurrent
				onCleared: {
					panels.popouts.close();
					openPanels.startmenu = false;
					openPanels.mprisViewer = false;
				}
			}

			InteractionHandler {
				screen: scope.modelData
				openPanels: openPanels
				popouts: panels.popouts
				bar: bar
				panels: panels

				// implicitWidth: bar.implicitWidth
				// implicitHeight: bar.implicitHeight

				// anchors.top: parent.top
				// anchors.left: parent.left
				// anchors.right: parent.right
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
				anchors.top: parent.top

				screen: scope.modelData
				openPanels: openPanels
				popouts: panels.popouts // qmllint disable incompatible-type
				panels: panels
				rootPanel: win

				Component.onCompleted: PanelService.bars.set(scope.modelData, this)
			}

			OSDWrapper.Wrapper {
				id: osdWrapper
				screen: scope.modelData
				bar: bar
				openPanels: openPanels
			}
		}
	}
}
