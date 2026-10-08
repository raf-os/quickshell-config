import qs.components

import org.nightshell.Configs

import QtQuick
import QtQuick.Layouts

ColumnLayout {
	id: root

	required property string label
	required property string text

	spacing: Styles.spacing_xs

	StyledText {
		id: labelText

		text: root.label
		font.pointSize: Styles.text_xs
		font.capitalization: Font.AllUppercase
		font.weight: 600
	}

	StyledText {
		id: textText

		text: root.text
		font.pointSize: Styles.text_md
		font.weight: 600
	}
}
