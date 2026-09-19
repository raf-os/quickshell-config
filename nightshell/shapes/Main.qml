pragma ComponentBehavior: Bound

import qs.modules.bar

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

	layer.enabled: true
	layer.effect: MultiEffect {
		blurEnabled: true
		blur: 1
		blurMax: 8
	}

	Rectangle {
		id: holes
		anchors.fill: parent
		visible: false
		layer.enabled: true
		color: "transparent"

		Item {
			id: barHoles

			anchors {
				top: parent.top
				horizontalCenter: parent.horizontalCenter
			}

			implicitWidth: 300
			implicitHeight: root.bar.height

			RowLayout {
				anchors {
					top: parent.top
					bottom: parent.bottom
					horizontalCenter: parent.horizontalCenter
				}

				spacing: 8

				implicitWidth: parent.implicitWidth

				Repeater {
					model: 5
					delegate: Rectangle {
						implicitWidth: 22
						implicitHeight: implicitWidth
						radius: implicitWidth / 2
						color: "white"
					}
				}
			}
		}
	}

	RenderPass {
		id: overlayGlow
		isBackground: false
		visible: false

		BarBg {
			bar: root.bar
			centerItem: barHoles
			maxWidth: root.bar.width
			maxHeight: root.bar.height
		}

		SystemIconBg {
			bar: root.bar
		}
	}

	MultiEffect {
		id: holePass
		source: overlayGlow
		anchors.fill: parent
		maskEnabled: true
		maskInverted: true
		maskSource: holes
		maskThresholdMin: 0.5
		maskSpreadAtMin: 0.75
	}

	component RenderPass: Shape {
		id: renderPassShape
		required property bool isBackground
		anchors.fill: parent
	}
}
