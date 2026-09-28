import qs.components

import org.nightshell.Notifications
import org.nightshell.Configs

import QtQuick
import QtQuick.Effects
import QtQuick.Layouts

ColumnLayout {
	id: root

	required property string title

	signal closeRequested(reason: int)

	Layout.fillWidth: true
	Layout.fillHeight: true

	clip: true
	spacing: Styles.spacing_sm

	RowLayout {
		id: headerWrapper
		Layout.fillWidth: true

		spacing: Styles.spacing_md

		// implicitHeight: Math.max(notificationTitle.height, closeButton.height)

		StyledText {
			id: notificationTitle
			text: root.title

			// anchors {
			// 	left: parent.left
			// 	right: closeButton.left
			// 	verticalCenter: parent.verticalCenter
			// }

			Layout.fillWidth: true
			Layout.fillHeight: true

			font.pointSize: Styles.text_md
			font.weight: 600

			elide: Text.ElideRight
		}

		MouseArea {
			id: closeButton

			readonly property int iconSize: Styles.text_md * 2
			readonly property int iconPadding: 1

			Layout.fillHeight: true

			implicitWidth: iconSize
			implicitHeight: iconSize

			cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
			hoverEnabled: true

			onClicked: {
				root.closeRequested(NotificationCloseReason.CloseRequested);
			}

			Rectangle {
				anchors.fill: parent
				color: closeButton.containsMouse ? Colors.primary : Colors.primaryContent
				border.width: 1
				border.color: Colors.primary
			}

			Image {
				id: closeIcon
				anchors.centerIn: parent
				width: closeButton.iconSize
				height: closeButton.iconSize
				source: `image://qicons/shell/close`
			}

			MultiEffect {
				source: closeIcon
				anchors.fill: closeIcon
				colorization: 1
				colorizationColor: closeButton.containsMouse ? Colors.primaryContent : Colors.primary
			}
		}
	}
}
