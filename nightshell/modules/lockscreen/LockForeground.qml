import qs.components

import org.nightshell.Components
import org.nightshell.Configs

import QtQuick

Item {
	id: root

	required property string bufferText
	required property string pamMessage
	property alias inputTextContent: textInput.text

	anchors.fill: parent

	signal pwTextChanged
	signal inputAccepted

	Item {
		id: centerComponent

		anchors.centerIn: parent
		implicitWidth: 400
		implicitHeight: 320

		Item {
			id: pamMessageWrapper

			implicitWidth: pamMessageMetrics.width > 320 ? 320 : pamMessageMetrics.width
			implicitHeight: root.pamMessage !== "" ? pamMessageText.height + pamMessageText.padding * 2 : 0

			clip: true

			anchors {
				bottom: inputWrapper.top
				bottomMargin: 4
			}

			Rectangle {
				anchors.fill: parent
				color: Colors.primaryContent
			}

			TextMetrics {
				id: pamMessageMetrics

				text: root.pamMessage
				elideWidth: 320
				font.family: Config.appearance.fontFamily.sans
				font.pointSize: Styles.text_md
			}

			Text {
				id: pamMessageText

				anchors {
					bottom: parent.bottom
					left: parent.left
					right: parent.right
				}

				text: pamMessageMetrics.text
				color: Colors.primary

				font: pamMessageMetrics.font
				padding: Styles.padding_md

				wrapMode: Text.Wrap
			}
		}

		Item {
			id: inputWrapper

			implicitWidth: 320
			implicitHeight: textInput.height

			anchors {
				centerIn: parent
			}

			Rectangle {
				anchors.fill: parent
				color: Colors.secondaryContent

				border.width: 2
				border.color: textInput.activeFocus ? Colors.secondary : "transparent"
			}

			TextInput {
				id: textInput

				anchors {
					left: parent.left
					right: parent.right
					verticalCenter: parent.verticalCenter
				}

				visible: false
				focus: true
				text: root.bufferText
				echoMode: TextInput.Password
				passwordCharacter: "*"

				font.family: Config.appearance.fontFamily.sans
				font.pointSize: Styles.text_md
				font.weight: 600
				font.letterSpacing: 2

				color: Colors.secondary
				selectionColor: Colors.secondary
				selectedTextColor: Colors.secondaryContent

				width: parent.implicitWidth
				padding: Styles.padding_md

				verticalAlignment: Text.AlignVCenter

				Keys.onPressed: ev => {
					if (ev.key === Qt.Key_Backspace && ev.modifiers & Qt.ControlModifier) {
						textInput.text = "";
						ev.accepted = true;
						root.pwTextChanged();
					}
				}

				onTextEdited: {
					root.pwTextChanged();
				}

				onAccepted: {
					root.inputAccepted();
				}
			}

			SimpleGlowEffect {
				source: textInput
				anchors.fill: textInput
				shadowColor: textInput.color
			}
		}

		SMouseArea {
			id: confirmArea

			anchors {
				left: inputWrapper.left
				right: inputWrapper.right
				top: inputWrapper.bottom
				topMargin: 4
			}

			implicitHeight: confirmText.height

			ChamferRect {
				id: confirmBg

				anchors.fill: parent
				color: Colors.secondary
				bottomRightChamfer: Math.floor(parent.height / 3)
			}

			StyledText {
				id: confirmText

				anchors {
					left: parent.left
					right: parent.right
					verticalCenter: parent.verticalCenter
				}

				text: "UNLOCK"
				color: Colors.secondaryContent

				font.pointSize: Styles.text_md
				font.weight: 600

				padding: Styles.padding_md
			}
		}
	}
}
