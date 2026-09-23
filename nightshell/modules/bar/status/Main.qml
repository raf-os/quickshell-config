import org.nightshell.Configs
import QtQuick
import QtQuick.Layouts

Item {
	id: root

	implicitWidth: mainLayout.width

	RowLayout {
		id: mainLayout

		anchors {
			top: parent.top
			bottom: parent.bottom
			horizontalCenter: parent.horizontalCenter
		}
	}
}
