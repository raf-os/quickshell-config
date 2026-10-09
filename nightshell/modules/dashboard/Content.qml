import qs.components

import org.nightshell.Configs
import org.nightshell.Utils

import QtQuick
import QtQuick.Layouts

MouseArea {
	id: root

	required property bool isActive

	readonly property int desiredWidth: 420

	property alias titleTransform: titleTransform
	property alias userInfoTransform: userInfoTransform

	implicitWidth: desiredWidth
	implicitHeight: mainLayout.height

	signal exitAnimationFinished

	onIsActiveChanged: {
		if (!isActive)
			root.exitAnimationFinished();
	}

	Background {
		anchors {
			top: mainLayout.top
			left: mainLayout.left
		}
		width: mainLayout.width
		height: mainLayout.height

		contentItem: root
	}

	ColumnLayout {
		id: mainLayout

		anchors {
			top: parent.top
			left: parent.left
			topMargin: Styles.spacing_md
			leftMargin: Styles.spacing_md * 2
		}

		width: root.desiredWidth

		Item {
			id: titleWrapper

			readonly property int padding: Styles.padding_sm

			Layout.fillWidth: true
			implicitHeight: titleText.height

			StyledText {
				id: titleText

				AbsoluteTransform {
					id: titleTransform
					targetAncestor: mainLayout
				}

				anchors {
					left: parent.left
					top: parent.top
				}

				padding: titleWrapper.padding

				text: "DASHBOARD"
				font.pointSize: Styles.text_md
				font.weight: 600
			}
		}

		UserInfo {
			id: userInfoComponent
			readonly property int padding: Styles.padding_md
			Layout.margins: padding

			AbsoluteTransform {
				id: userInfoTransform
				targetAncestor: mainLayout
				padding: Qt.vector2d(userInfoComponent.padding * 2, userInfoComponent.padding * 2)
			}
		}
	}
}
