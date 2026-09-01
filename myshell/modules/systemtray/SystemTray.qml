pragma ComponentBehavior: Bound

import MyShellPlugin
import MyShellPlugin.Utils
import MyShellPlugin.Configs
import org.nightshell.SystemTray
import QtQuick
import QtQuick.Layouts

Item {
	id: root

	readonly property int padding: 4

	Layout.fillHeight: true

	implicitWidth: mainLayout.width + padding * 2

	Rectangle {
		anchors.fill: parent
		color: Colors.colors.base0
		radius: Config.appearance.rounding.sm
	}

	RowLayout {
		id: mainLayout

		anchors {
			top: parent.top
			bottom: parent.bottom
			horizontalCenter: parent.horizontalCenter
		}

		Repeater {
			model: SystemTrayQML.items

			delegate: Item {
				id: stroot
				required property StatusNotifierItem modelData

				Layout.topMargin: root.padding
				Layout.bottomMargin: root.padding
				Layout.fillHeight: true

				implicitWidth: height

				Image {
					anchors.centerIn: parent
					asynchronous: true
					width: parent.width
					height: parent.height
					source: stroot.modelData.iconUrl
				}
			}
		}
	}
}
