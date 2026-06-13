pragma ComponentBehavior: Bound

import qs.components
import MyShellPlugin
import MyShellPlugin.Configs
import Quickshell
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects

Item {
	id: root

	required property int currentState
	property int padding: 10

	readonly property int fontPointsize: Config.appearance.fontSize.lg
	readonly property string fontFamilyName: Config.appearance.fontFamily.sans
	readonly property int fontWeightAmt: 600
	readonly property int leftInset: Config.appearance.spacing.xl

	property bool isCompleted: false

	signal requestPanelClose

	Layout.fillWidth: true
	implicitHeight: txtMetric.height + padding * 2

	clip: true

	Component.onCompleted: {
		updateTitle();
		delayedStart.start();
	}

	Timer {
		id: delayedStart
		interval: 1
		onTriggered: {
			root.isCompleted = true;
		}
	}

	onCurrentStateChanged: updateTitle()

	function updateTitle() {
		const comp = mapComponent(root.currentState);
		sview.replaceCurrentItem(comp);
	}

	function mapComponent(idx: int): Component {
		switch (idx) {
		case 0:
			return appListTitle;
		case 1:
			return cmdListTitle;
		default:
			return appListTitle;
		}
	}

	StackView {
		id: sview

		anchors {
			top: parent.top
			left: parent.left
			right: parent.right
		}

		implicitHeight: parent.implicitHeight
		clip: true

		initialItem: appListTitle

		replaceEnter: Transition {
			enabled: root.isCompleted

			ParallelAnimation {
				NAnim {
					property: "y"
					from: -48
					to: 0
					duration: 200
				}
				NAnim {
					property: "opacity"
					from: 0
					to: 1
					duration: 200
				}
			}
		}

		replaceExit: Transition {
			enabled: root.isCompleted

			ParallelAnimation {
				NAnim {
					property: "y"
					from: 0
					to: 48
					duration: 200
				}
				NAnim {
					property: "opacity"
					from: 1
					to: 0
					duration: 200
				}
			}
		}
	}

	RowLayout {
		id: buttonsList

		anchors {
			top: parent.top
			bottom: parent.bottom
			right: parent.right
			rightMargin: Config.appearance.padding.md
		}

		TitleButton {
			icon: "󰒓"

			function onClicked() {
				Quickshell.execDetached("myshell-control-panel");
				root.requestPanelClose();
			}
		}
	}

	TextMetrics {
		id: txtMetric
		text: "Lorem Ipsum Dolor Sit Amet"
		font.pointSize: root.fontPointsize
		font.family: root.fontFamilyName
	}

	Component {
		id: appListTitle

		TWrapper {
			text: "Applications"
		}
	}

	Component {
		id: cmdListTitle

		TWrapper {
			text: "User Commands"
		}
	}

	component TitleButton: Item {
		id: titleButton

		property int size: Config.appearance.fontSize.xl
		property int padding: Config.appearance.padding.xs
		required property string icon

		implicitWidth: size + padding * 2
		implicitHeight: size + padding * 2

		function onClicked() {
		}

		MouseArea {
			id: titleButtonInteractive

			enabled: titleButton.enabled
			anchors.fill: parent
			cursorShape: titleButton.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
			hoverEnabled: true

			onClicked: {
				titleButton.onClicked();
			}
		}

		RectangularShadow {
			anchors.fill: parent
			color: Qt.rgba(0, 0, 0, 0.35)

			offset.y: 2
			radius: titleButtonBg.radius / 2

			blur: 8
			spread: -1
		}

		Rectangle {
			id: titleButtonBg
			anchors.fill: parent

			color: titleButton.enabled ? (titleButtonInteractive.containsMouse ? Colors.colors.primary : Colors.colors.base2) : Colors.colors.base0
			radius: Config.appearance.rounding.sm

			Behavior on color {
				ColorAnimation {
					duration: 100
				}
			}
		}

		StyledText {
			anchors.fill: parent
			text: titleButton.icon

			font.family: Config.appearance.fontFamily.mono
			font.pixelSize: titleButton.size
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter

			color: titleButton.enabled ? Colors.colors.baseContent : Colors.colors.baseContentMuted
		}
	}

	component TWrapper: Item {
		required property string text

		implicitWidth: StackView.view ? StackView.view.width : 0
		implicitHeight: StackView.view ? StackView.view.height : 0

		Text {
			anchors.fill: parent
			text: parent.text

			font.pointSize: root.fontPointsize
			font.family: root.fontFamilyName
			font.weight: root.fontWeightAmt
			elide: Text.ElideRight

			color: Colors.colors.baseContent

			leftPadding: root.leftInset

			verticalAlignment: Text.AlignVCenter
		}
	}
}
