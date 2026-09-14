pragma ComponentBehavior: Bound

import qs.components
import qs.services
import MyShellPlugin
import MyShellPlugin.Configs
import org.nightshell.SystemTray
import QtQuick
import QtQuick.Effects

MouseArea {
	id: root

	acceptedButtons: Qt.AllButtons
	hoverEnabled: true

	required property StatusNotifierItem modelData
	required property bool isActive

	property bool isTooltipShowing: false

	implicitWidth: height
	cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

	signal hideTooltip
	signal openMenu

	Timer {
		id: hoverTimeout
		interval: 500
		onTriggered: {
			root.isTooltipShowing = true;
		}
	}

	onEntered: {
		hoverTimeout.start();
	}

	onExited: {
		hoverTimeout.stop();
		root.hideTooltip();
	}

	onClicked: ev => {
		hoverTimeout.stop();
		root.hideTooltip();

		if (ev.button === Qt.LeftButton) {
			if (root.modelData.isMenuOnly)
				root.openMenu();
			else
				root.modelData.activate();
		} else if (ev.button === Qt.MiddleButton) {
			if (root.modelData.isMenuOnly)
				return;
			root.modelData.secondaryActivate();
		} else if (ev.button === Qt.RightButton) {
			root.openMenu();
		}
	}

	Image {
		id: trayIcon

		asynchronous: true
		width: parent.width
		height: parent.height

		source: root.modelData.iconUrl
		scale: root.isActive ? 1.2 : (root.pressed ? 0.8 : 1)

		layer.enabled: true
		layer.effect: MultiEffect {
			// saturation: root.isActive ? 0 : -1
			colorization: root.isActive ? 0 : 1
			colorizationColor: Colors.colors.baseContent

			Behavior on colorization {
				NAnim {
					duration: 300
				}
			}
		}

		Behavior on scale {
			NAnim {
				duration: 150
			}
		}
	}

	Loader {
		id: tooltipLoader
		active: root.isTooltipShowing

		anchors {
			top: parent.bottom
			horizontalCenter: parent.horizontalCenter
		}

		sourceComponent: Item {
			id: tooltipItem

			readonly property int animDuration: 200
			readonly property int offsetDistance: 8

			anchors {
				top: parent.top
				topMargin: tooltipItem.offsetDistance
				horizontalCenter: parent.horizontalCenter
			}

			implicitWidth: tooltipText.width + (Config.appearance.padding.md * 2)
			implicitHeight: tooltipText.height + (Config.appearance.padding.xs * 2)

			Component.onCompleted: {
				playEnterAnim();
			}

			function playEnterAnim() {
				exitAnim.stop();
				enterAnim.start();
			}

			function playExitAnim() {
				enterAnim.stop();
				exitAnim.start();
			}

			Loader {
				active: !GameModeService.isGamemode
				anchors.fill: bgRect
				sourceComponent: RectangularShadow {
					// anchors.fill: bgRect
					blur: 4
					spread: 2

					offset.y: 2

					radius: bgRect.radius
					color: Qt.rgba(0, 0, 0, 0.5)
				}
			}

			Rectangle {
				id: bgRect
				anchors.fill: parent
				color: Colors.colors.base2
				radius: Config.appearance.rounding.md
			}

			StyledText {
				id: tooltipText
				anchors.centerIn: parent

				text: root.modelData.title

				font.pointSize: Config.appearance.fontSize.xs
				font.family: Config.appearance.fontFamily.sans
			}

			Connections {
				target: root
				function onHideTooltip() {
					tooltipItem.playExitAnim();
				}
			}

			ParallelAnimation {
				id: enterAnim

				NAnim {
					target: tooltipItem
					property: "anchors.topMargin"
					from: tooltipItem.offsetDistance / 2
					to: tooltipItem.offsetDistance
					duration: tooltipItem.animDuration
				}

				NAnim {
					target: tooltipItem
					property: "opacity"
					from: 0
					to: 1
					duration: tooltipItem.animDuration
				}
			}

			SequentialAnimation {
				id: exitAnim

				ParallelAnimation {
					NAnim {
						target: tooltipItem
						property: "opacity"
						to: 0
						duration: tooltipItem.animDuration
					}

					NAnim {
						target: tooltipItem
						property: "anchors.topMargin"
						to: tooltipItem.offsetDistance / 2
						duration: tooltipItem.animDuration
					}
				}
				ScriptAction {
					script: root.isTooltipShowing = false
				}
			}
		}
	}
}
