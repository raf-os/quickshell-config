pragma ComponentBehavior: Bound

import qs.components
import MyShellPlugin
import MyShellPlugin.Configs
import org.nightshell.SystemTray
import Quickshell
import Quickshell.Hyprland
import QtQuick
import QtQuick.Controls

Item {
	id: root

	required property SystemTray systemTray

	required property int systemTrayRelativePosition

	readonly property int padding: Config.appearance.padding.sm
	readonly property int animDuration: 300
	readonly property int popoutWidth: 240

	property alias exclusionRegion: exclusionRegion
	property alias isStackViewBusy: popoutCurrent.busy

	property int xOffset
	property bool enablePostInitialAnim: false

	function pushIndex(index: var, depth: int, statusItem: StatusNotifierItem) {
		popoutCurrent.push(rootSubmenuComponent, {
			statusItem: statusItem,
			depth: depth + 1,
			rootIndex: index
		});
	}

	anchors {
		topMargin: root.systemTray.current ? 4 : 12
	}

	Behavior on anchors.topMargin {
		NAnim {
			duration: root.animDuration
		}
	}

	Behavior on x {
		enabled: root.enablePostInitialAnim
		NAnim {
			duration: root.animDuration
		}
	}

	HyprlandFocusGrab {
		active: root.systemTray.current
		windows: [QsWindow.window]
		onCleared: {
			root.systemTray.closePopup();
		}
	}

	Region {
		id: exclusionRegion

		x: root.x - popoutMenuContent.width / 2
		y: root.y
		width: popoutMenuContent.width
		height: root.systemTray.current ? (root.anchors.topMargin + popoutMenuContent.implicitHeight) : 0
	}

	Connections {
		target: root.systemTray
		function onCurrentActiveChanged() {
			if (!root.systemTray.currentActive)
				return;

			const item = root.systemTray.currentActive;
			root.xOffset = item.x + item.width / 2;
		}

		function onCurrentChanged() {
			if (root.systemTray.current) {
				root.enablePostInitialAnim = true;
			} else {
				root.enablePostInitialAnim = false;
			}
		}
	}

	Item {
		id: popoutMenuContent

		anchors {
			top: parent.bottom
			horizontalCenter: parent.horizontalCenter
		}

		implicitWidth: root.popoutWidth
		implicitHeight: popoutCurrent.currentItem ? popoutCurrent.currentItem.implicitHeight : 0

		opacity: root.systemTray.current ? 1 : 0

		Behavior on implicitHeight {
			enabled: root.enablePostInitialAnim
			NAnim {
				duration: root.animDuration
			}
		}

		Behavior on opacity {
			NAnim {
				duration: root.animDuration
			}
		}

		Rectangle {
			anchors.fill: parent
			color: Colors.colors.base0
			radius: Config.appearance.rounding.sm
		}

		StackView {
			id: popoutCurrent

			readonly property int animDuration: 300

			anchors.fill: parent
			clip: true

			Connections {
				target: root.systemTray

				function onCurrentChanged() {
					if (root.systemTray.current) {
						popoutCurrent.replace(rootSubmenuComponent, {
							statusItem: root.systemTray.current,
							rootIndex: root.systemTray.current.menuHandle.rootIndex,
							depth: 0
						});
					} else {
						popoutCurrent.clear(StackView.PopTransition);
					}
				}
			}

			popExit: Transition {
				ParallelAnimation {
					NAnim {
						property: "opacity"
						from: 1
						to: 0
						duration: popoutCurrent.animDuration
					}

					NAnim {
						property: "y"
						from: 0
						to: 16
						duration: popoutCurrent.animDuration
					}
				}
			}

			replaceEnter: Transition {
				ParallelAnimation {
					NAnim {
						property: "opacity"
						from: 0
						to: 1
						duration: popoutCurrent.animDuration
					}
					XAnimator {
						from: 64
						to: 0
						duration: popoutCurrent.animDuration
						easing.type: Easing.BezierSpline
						easing.bezierCurve: Config.appearance.animCurves.defaultEase
					}
				}
			}

			replaceExit: Transition {
				ParallelAnimation {
					NAnim {
						property: "opacity"
						to: 0
						duration: popoutCurrent.animDuration
					}
					XAnimator {
						from: 0
						to: -64
						duration: popoutCurrent.animDuration
						easing.type: Easing.BezierSpline
						easing.bezierCurve: Config.appearance.animCurves.defaultEase
					}
				}
			}
		}
	}

	Component {
		id: rootSubmenuComponent
		RootSubmenu {
			implicitWidth: StackView.view ? StackView.view.width : 0

			onNavigateToIndex: (index, statusItem) => {
				root.pushIndex(index, depth, statusItem);
			}

			onNavigateBackwards: {
				if (popoutCurrent.depth > 1) {
					popoutCurrent.popCurrentItem();
				} else {
					popoutCurrent.clear(StackView.PopTransition);
				}
			}

			onRequestClose: root.systemTray.closePopup()
		}
	}
}
