import org.nightshell.Wallpaper
import QtQuick

Item {
	id: root

	anchors.fill: parent

	Loader {
		active: WallpaperManager.current
		anchors.fill: parent

		sourceComponent: Image {
			anchors.fill: parent
			asynchronous: true

			source: WallpaperManager.current.path
			fillMode: Image.PreserveAspectCrop
		}
	}
}
