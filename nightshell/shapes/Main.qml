pragma ComponentBehavior: Bound

import qs.modules.bar
import qs.modules.bar.workspaces

import Quickshell
import QtQuick
import QtQuick.Shapes
import QtQuick.Effects
import QtQuick.Layouts

Item {
	id: root

	required property ShellScreen screen
	required property Bar bar

	width: screen.width
	height: screen.height

	MultiEffect {
		id: glowEffect
		source: glowLayer
		anchors.fill: glowLayer
		blurEnabled: true
		blur: 1
		blurMax: 12
		brightness: 0.2
	}

	RenderPass {
		id: glowLayer
		isBackground: false

		BarBg {
			bar: root.bar
			width: root.bar.width
			height: root.bar.height
		}

		BarLeftBg {
			bar: root.bar
		}
	}

	// MultiEffect {
	// 	id: holePass
	// 	source: overlayGlow
	// 	anchors.fill: parent
	// 	maskEnabled: true
	// 	maskInverted: true
	// 	maskSource: holes
	// 	maskThresholdMin: 0.5
	// 	maskSpreadAtMin: 0.75
	// }

	component RenderPass: Shape {
		id: renderPassShape
		required property bool isBackground
		anchors.fill: parent
	}
}
