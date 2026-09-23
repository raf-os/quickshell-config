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

	implicitWidth: mainLayout.width + Styles.padding_md * 2

	Layout.fillHeight: true

	TextMetrics {
		id: dateMetrics
		font.family: root.clockFont
		font.weight: 400
		font.pointSize: Math.max(root.height * 0.35, Styles.text_sm)
		text: Time.dateStr
	}

	TextMetrics {
		id: timeMetrics
		font.family: root.clockFont
		font.weight: 600
		font.pointSize: Math.max(root.height * 0.5, Styles.text_m)
		text: Time.timeStr
	}

	BarGlowEffect {
		source: mainLayout
		anchors.fill: mainLayout
	}

	RowLayout {
		id: mainLayout
		spacing: Styles.spacing_sm

		anchors {
			verticalCenter: parent.verticalCenter
			horizontalCenter: parent.horizontalCenter
		}

		StyledText {
			text: dateMetrics.text
			font: dateMetrics.font
			color: root.textColor
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
