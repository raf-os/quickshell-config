import qs.components

import org.nightshell.Configs

import QtQuick
import QtQuick.Layouts

MouseArea {
	id: root

	required property bool isActive

	readonly property int desiredWidth: 420

	property alias titleWrapper: titleWrapper

	implicitWidth: desiredWidth
	implicitHeight: mainLayout.height

	signal exitAnimationFinished

	onIsActiveChanged: {
		if (!isActive)
			root.exitAnimationFinished();
	}

	Background {
		anchors.centerIn: mainLayout
		width: mainLayout.width
		height: mainLayout.height

		contentItem: root
	}

	ColumnLayout {
		id: mainLayout

		anchors {
			top: parent.top
			left: parent.left
		}

		width: root.desiredWidth

		Item {
			id: titleWrapper

			readonly property int padding: Styles.padding_sm

			Layout.fillWidth: true
			implicitHeight: titleText.height + padding * 2

			StyledText {
				id: titleText

				anchors {
					left: parent.left
					leftMargin: titleWrapper.padding
					right: parent.right
					rightMargin: titleWrapper.padding
					verticalCenter: parent.verticalCenter
				}

				text: "DASHBOARD"
				font.pointSize: Styles.text_md
				font.weight: 600
			}
		}

		UserInfo {}
	}
}
