import qs.components
import QtQuick

BaseStatus {
	id: root

	StyledText {
		anchors.fill: parent

		font.weight: 700
		font.pixelSize: parent.height * 0.6
		text: "us"

		verticalAlignment: Text.AlignVCenter
	}
}
