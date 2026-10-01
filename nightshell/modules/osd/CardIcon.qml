import qs.components

import org.nightshell.Configs

import QtQuick
import QtQuick.Layouts

Item {
	id: root

	required property string icon
	property int iconSize: 48

	implicitWidth: iconSize + 12

	Layout.fillHeight: true

	ShellIcon {
		id: shellIcon
		anchors.centerIn: parent
		size: root.iconSize
		icon: root.icon
		colorize: true
		color: Colors.primary
	}
}
