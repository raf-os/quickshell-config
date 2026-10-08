import qs.components

import org.nightshell.Services
import org.nightshell.Utils
import org.nightshell.Configs

import QtQuick
import QtQuick.Layouts

Item {
	id: root

	implicitHeight: mainLayout.height

	Rectangle {
		anchors.fill: parent
	}

	RowLayout {
		id: mainLayout

		anchors {
			left: parent.left
			right: parent.right
			top: parent.top
		}

		Item {
			id: userImageWrapper

			implicitWidth: 96
			implicitHeight: 96

			Layout.fillHeight: true
			Layout.alignment: Qt.AlignVCenter

			Rectangle {
				anchors.fill: parent
			}
		}

		Item {
			id: userInfoContent

			Layout.fillWidth: true
			Layout.fillHeight: true

			implicitHeight: userInfoContentLayout.implicitHeight

			ColumnLayout {
				id: userInfoContentLayout

				implicitWidth: parent.implicitWidth
				spacing: Styles.spacing_md

				InfoLabelPair {
					label: "NAME"
					text: UserData.name
				}

				InfoLabelPair {
					label: "Operating System"
					text: SysInfo.name
				}

				InfoLabelPair {
					label: "Uptime"
					text: "1 century"
				}
			}
		}
	}
}
