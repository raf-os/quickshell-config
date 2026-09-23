import org.nightshell.Utils
import QtQuick

MouseArea {
	id: root

	onClicked: {
		FocusGrabberManager.forceClear();
	}
}
