pragma ComponentBehavior: Bound

import qs.components

import org.nightshell.Configs
import org.nightshell.DesktopEntries

import QtQuick

Item {
	id: root

	property int size: 48
	property string appName
	property string appIcon
	property string imageUrl
	readonly property DesktopEntry desktopEntry: appName === "" ? null : EntryManager.findEntryById(appName)
	readonly property string resolvedAppIcon: {
		if (root.appIcon === "") {
			if (root.desktopEntry)
				return desktopEntry.icon;
			else
				return "";
		} else {
			return root.appIcon;
		}
	}

	implicitWidth: size
	implicitHeight: size

	Loader {
		id: systemIconLoader
		active: (root.desktopEntry || root.appIcon !== "") && root.imageUrl === ""
		anchors.fill: parent

		sourceComponent: Image {
			asynchronous: true
			source: `image://qicons/qt/${root.resolvedAppIcon}`
			width: root.size
			height: root.size
			anchors.centerIn: parent
		}
	}

	Loader {
		id: emptyIconLoader
		active: root.imageUrl === "" && root.appIcon === ""
		anchors.fill: parent
		sourceComponent: Item {
			anchors.fill: parent

			Rectangle {
				anchors.fill: parent
				color: Colors.primaryContent
				border.width: 1
				border.color: Colors.primary
			}

			StyledText {
				anchors.fill: parent
				text: "?"
				color: Colors.primary
				font.pixelSize: height * 0.8

				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignHCenter
			}
		}
	}

	Loader {
		id: imagePixmapLoader
		active: root.imageUrl !== ""
		anchors.fill: parent
		sourceComponent: Item {
			anchors.fill: parent

			Image {
				anchors.centerIn: parent
				asynchronous: true
				width: parent.width
				height: parent.height
				source: root.imageUrl
			}
		}
	}
}
