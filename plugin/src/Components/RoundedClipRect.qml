pragma ComponentBehavior: Bound

import QtQuick

Item {
	id: root

	default property alias data: container.data
	property alias children: container.children

	property alias antialiasing: rectangle.antialiasing
	property alias radius: rectangle.radius
	property alias topLeftRadius: rectangle.topLeftRadius
	property alias topRightRadius: rectangle.topRightRadius
	property alias bottomLeftRadius: rectangle.bottomLeftRadius
	property alias bottomRightRadius: rectangle.bottomRightRadius

	Rectangle {
		id: rectangle
		anchors.fill: root
		color: "#ffffff00"
		layer.enabled: true
		visible: false
	}

	Item {
		id: container
		anchors.fill: root
	}

	ShaderEffectSource {
		id: shaderSource
		hideSource: true
		sourceItem: container
	}

	ShaderEffect {
		id: shader
		anchors.fill: root
		fragmentShader: `qrc:/Nightshell/Components/shaders/rectclipmask.frag.qsb`
		property Rectangle rect: rectangle
		property ShaderEffectSource content: shaderSource
	}
}
