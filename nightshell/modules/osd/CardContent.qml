pragma ComponentBehavior: Bound

import qs.components

import org.nightshell.Configs

import QtQuick
import QtQuick.Layouts

ColumnLayout {
	id: root

	required property string title
	property string content

	spacing: Styles.spacing_md

	Layout.fillWidth: true
	Layout.fillHeight: true

	StyledText {
		id: titleComponent

		Layout.fillWidth: true
		text: root.title
		color: Colors.primary

		font.pointSize: Styles.text_md
		font.weight: 700
	}

	Loader {
		id: contentLoader
		Layout.fillWidth: true

		active: root.content !== ""

		sourceComponent: StyledText {
			id: contentComponent

			text: root.content
			color: Colors.primary

			font.pointSize: Styles.text_sm
		}
	}
}
