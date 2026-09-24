import org.nightshell.Configs
import org.nightshell.Utils
import QtQuick
import QtQuick.Effects

MouseArea {
	id: root

	implicitWidth: height

	Image {
		id: iconImage
		asynchronous: true
		visible: false
		width: parent.width
		height: parent.height
		source: `image://qicons/qt/${SysInfo.iconName}`
	}

	MultiEffect {
		source: iconImage
		anchors.fill: iconImage
		colorization: 1
		colorizationColor: Colors.secondaryContent
		// brightness: 0.5
	}
}
