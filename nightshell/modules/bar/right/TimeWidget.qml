import qs.components
import qs.modules.bar

import org.nightshell.Services
import org.nightshell.Configs
import QtQuick
import QtQuick.Layouts
import QtQuick.Effects

BaseWidget {
	id: root

	readonly property string clockFont: Config.appearance.fontFamily.sans
	readonly property color textColor: Colors.primary

	implicitWidth: (dateFont.averageCharacterWidth * dateMetrics.text.length) + (timeFont.averageCharacterWidth * timeMetrics.text.length) + mainLayout.spacing + Styles.padding_md * 2

	Layout.fillHeight: true

	FontMetrics {
		id: dateFont
		font.family: root.clockFont
		font.weight: 400
		font.pointSize: Math.max(root.height * 0.35, Styles.text_sm)
	}

	FontMetrics {
		id: timeFont
		font.family: root.clockFont
		font.weight: 600
		font.pointSize: Math.max(root.height * 0.5, Styles.text_m)
	}

	TextMetrics {
		id: dateMetrics
		font: dateFont.font
		text: Time.dateStr
	}

	TextMetrics {
		id: timeMetrics
		font: timeFont.font
		text: Time.timeStr
	}

	SimpleGlowEffect {
		source: mainLayout
		anchors.fill: mainLayout
		shadowColor: root.textColor
	}

	RowLayout {
		id: mainLayout
		visible: false
		spacing: Styles.spacing_sm

		anchors {
			verticalCenter: parent.verticalCenter
			horizontalCenter: parent.horizontalCenter
		}

		StyledText {
			text: dateMetrics.text
			font: dateMetrics.font
			color: Qt.darker(root.textColor, 1.3)
			Layout.alignment: Qt.AlignTop
		}

		StyledText {
			text: timeMetrics.text
			font: timeMetrics.font
			color: root.textColor
			Layout.alignment: Qt.AlignTop
		}
	}
}
