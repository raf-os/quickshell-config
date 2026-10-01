import org.nightshell.Utils
import QtQuick

MouseArea {
	id: root

	acceptedButtons: Qt.AllButtons

	onClicked: {
		FocusGrabberManager.forceClear();
	}

	focus: true
}
