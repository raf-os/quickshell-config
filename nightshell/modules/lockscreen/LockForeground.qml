import qs.components
import qs.components.icons

import org.nightshell.Components
import org.nightshell.Configs
import org.nightshell.Services
import org.nightshell.Utils

import QtQuick
import QtQuick.Layouts

Item {
	id: root

	required property LockAuth lockAuth
	readonly property string errorMessage: lockAuth.messageIsError ? lockAuth.message : ""
	property string bufferedErrorMessage
	property alias inputTextContent: textInput.text

	anchors.fill: parent

	signal pwTextChanged
	signal inputAccepted

	onErrorMessageChanged: {
		if (errorMessage === "")
			return;
		bufferedErrorMessage = errorMessage;
	}

	ColumnLayout {
		id: dateTimeLayout

		readonly property int hPadding: 96
		readonly property int vPadding: 64

		anchors {
			top: parent.top
			topMargin: dateTimeLayout.vPadding
			right: parent.right
			rightMargin: dateTimeLayout.hPadding
		}

		StyledText {
			id: dateComponent

			text: Qt.formatDate(SystemTime.date, Qt.TextDate)
			font.pointSize: Styles.text_lg

			Layout.alignment: Qt.AlignRight
		}

		StyledText {
			id: timeComponent

			text: Qt.formatDateTime(SystemTime.date, "hh:mm")
			font.pointSize: Styles.text_xl * 2

			Layout.alignment: Qt.AlignRight
		}
	}

	Item {
		id: userInfoComponent

		implicitWidth: userLockIcon.height + userInfoName.height

		anchors {
			bottom: centerComponent.top
			horizontalCenter: centerComponent.horizontalCenter
		}

		UserLockIcon {
			id: userLockIcon
			anchors {
				bottom: userInfoName.top
				bottomMargin: Styles.spacing_md
				horizontalCenter: parent.horizontalCenter
			}
			size: 96
			color: Colors.primary
			horizontalAlignment: Qt.AlignHCenter
			verticalAlignment: Qt.AlignBottom
		}

		StyledText {
			id: userInfoName
			anchors {
				bottom: parent.bottom
				horizontalCenter: parent.horizontalCenter
			}
			text: UserData.name
			font.pointSize: Styles.text_lg
			font.weight: 600
			color: Colors.primary
		}
	}

	Item {
		id: centerComponent

		anchors.centerIn: parent
		implicitWidth: 400
		implicitHeight: 320

		Item {
			id: pamMessageWrapper

			property int yOffset: 0
			opacity: 0

			implicitWidth: Math.min(pamMessageMetrics.width + pamMessageText.padding * 2 + 1, parent.implicitWidth)
			implicitHeight: (root.errorMessage !== "") ? pamMessageText.height : 0

			clip: true

			states: State {
				name: "active"
				when: root.errorMessage !== ""
				PropertyChanges {
					pamMessageWrapper.implicitHeight: pamMessageText.height
					pamMessageWrapper.yOffset: Styles.spacing_md
					pamMessageWrapper.opacity: 1
				}
			}

			transitions: [
				Transition {
					to: "active"
					NAnim {
						target: pamMessageWrapper
						properties: "yOffset,opacity"
						duration: 300
					}
				},
				Transition {
					to: ""
					NAnim {
						target: pamMessageWrapper
						properties: "yOffset,opacity"
						duration: 300
					}
				}
			]

			anchors {
				bottom: inputWrapper.top
				bottomMargin: pamMessageWrapper.yOffset
				horizontalCenter: parent.horizontalCenter
			}

			Rectangle {
				anchors.fill: parent
				color: Colors.primaryContent
			}

			TextMetrics {
				id: pamMessageMetrics

				text: root.bufferedErrorMessage
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

			implicitWidth: parent.implicitWidth
			implicitHeight: textInput.height

			anchors {
				centerIn: parent
			}

			Rectangle {
				anchors.fill: parent
				color: root.lockAuth.lockout ? Colors.primaryMuted : Colors.secondaryContent

				border.width: 2
				border.color: textInput.activeFocus ? Colors.secondary : "transparent"
			}

			TextInput {
				id: textInput
				enabled: !root.lockAuth.pending && !root.lockAuth.lockout

				anchors {
					left: parent.left
					right: parent.right
					verticalCenter: parent.verticalCenter
				}

				visible: false
				focus: true
				text: root.lockAuth.buffer
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

			enabled: !root.lockAuth.pending && !root.lockAuth.lockout
			anchors {
				left: inputWrapper.left
				right: inputWrapper.right
				top: inputWrapper.bottom
				topMargin: Styles.spacing_sm
			}

			implicitHeight: confirmText.height

			ChamferRect {
				id: confirmBg

				anchors.fill: parent
				color: Qt.darker(root.lockAuth.lockout ? Colors.primaryMuted : Colors.secondary, confirmArea.enabled ? 0 : 2)
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
				color: root.lockAuth.lockout ? Colors.primaryContent : Colors.secondaryContent

				font.pointSize: Styles.text_md
				font.weight: 600

				padding: Styles.padding_md
			}
		}

		ColumnLayout {
			id: lockoutWarningWrapper

			clip: true
			visible: root.lockAuth.lockout

			anchors {
				top: confirmArea.bottom
				topMargin: Styles.spacing_md
				left: parent.left
				right: parent.right
			}

			StyledText {
				text: "SESSION LOCKED"
				color: Colors.primary

				Layout.fillWidth: true
				font.pointSize: Styles.text_md
				font.weight: 600
				horizontalAlignment: Text.AlignHCenter
			}

			StyledText {
				id: lockoutWarningText

				Layout.fillWidth: true

				text: `${root.lockAuth.lockoutTimeLeft} minutes remaining`
				font.pointSize: Styles.text_sm

				horizontalAlignment: Text.AlignHCenter
			}
		}
	}
}
